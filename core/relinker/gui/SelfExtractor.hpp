#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace GuiSelf {
namespace fs = std::filesystem;
enum class Kind { Elf, Self, Other };
inline std::uint16_t U16(const std::uint8_t* p) { return std::uint16_t(p[0]) | (std::uint16_t(p[1]) << 8); }
inline std::uint32_t U32(const std::uint8_t* p) { return std::uint32_t(U16(p)) | (std::uint32_t(U16(p+2)) << 16); }
inline std::uint64_t U64(const std::uint8_t* p) { return std::uint64_t(U32(p)) | (std::uint64_t(U32(p+4)) << 32); }
inline void Put16(std::uint8_t* p, std::uint16_t v) { p[0]=std::uint8_t(v); p[1]=std::uint8_t(v>>8); }
inline void Put64(std::uint8_t* p, std::uint64_t v) { for(unsigned i=0;i<8;++i) p[i]=std::uint8_t(v>>(i*8)); }
inline bool Fits(std::uint64_t off, std::uint64_t n, std::uint64_t len) { return off<=len && n<=len-off; }
inline void Read(std::ifstream& f, std::uint64_t off, void* dst, std::size_t n) {
    f.clear();
    f.seekg(static_cast<std::streamoff>(off));
    f.read(static_cast<char*>(dst),static_cast<std::streamsize>(n));
    if (!f || f.gcount()!=static_cast<std::streamsize>(n)) throw std::runtime_error("Cannot read SELF data");
}
inline Kind Detect(const fs::path& file) {
    std::ifstream f(file,std::ios::binary);
    if(!f) throw std::runtime_error("Cannot open file: "+file.string());
    std::array<std::uint8_t,4> b{};
    f.read(reinterpret_cast<char*>(b.data()), b.size());
    if(f.gcount()!=static_cast<std::streamsize>(b.size())) return Kind::Other;
    auto m=U32(b.data());
    if(m==0x464c457f) return Kind::Elf;
    return m==0x1d3d154f || m==0xeef51454 ? Kind::Self : Kind::Other;
}
inline void Extract(const fs::path& source, const fs::path& target) {
    constexpr std::uint64_t MaxSize=8ull<<30, MaxTail=64ull<<20;
    std::ifstream f(source,std::ios::binary|std::ios::ate);
    if(!f) throw std::runtime_error("Cannot read SELF file");
    const auto sizePosition=f.tellg();
    if(sizePosition<32) throw std::runtime_error("Truncated SELF header");
    const auto size=static_cast<std::uint64_t>(sizePosition);
    std::array<std::uint8_t,32> sh{};
    Read(f,0,sh.data(),sh.size());
    const auto magic=U32(sh.data());
    if((magic!=0x1d3d154f && magic!=0xeef51454) || sh[6]!=1)
        throw std::runtime_error("Unsupported SELF header");
    const auto count=U16(sh.data()+24);
    const auto declared=U64(sh.data()+16);
    const std::uint64_t elfbase=32ull+32ull*count;
    if(count==0 || count>1024 || !Fits(elfbase,64,size) || declared>size || declared<elfbase+64)
        throw std::runtime_error("Invalid SELF size or entry count");
    std::vector<std::uint8_t> entries(count*32ull);
    Read(f,32,entries.data(),entries.size());
    std::array<std::uint8_t,64> eh{};
    Read(f,elfbase,eh.data(),eh.size());
    if(U32(eh.data())!=0x464c457f || eh[4]!=2 || eh[5]!=1 || eh[6]!=1 || U16(eh.data()+18)!=62 || U32(eh.data()+20)!=1)
        throw std::runtime_error("SELF does not embed supported ELF64 x86-64");
    const auto phoff=U64(eh.data()+32);
    const auto phsize=U16(eh.data()+54);
    const auto phcount=U16(eh.data()+56);
    if(phsize!=56 || phcount==0 || phcount>4096 || phoff<64 || phoff>(4ull<<20))
        throw std::runtime_error("Invalid ELF program header table");
    const auto phbytes=phoff+std::uint64_t(phsize)*phcount;
    if(phbytes>(4ull<<20) || !Fits(elfbase,phbytes,declared))
        throw std::runtime_error("Truncated ELF program headers");
    std::vector<std::uint8_t> elfhead(static_cast<std::size_t>(phbytes));
    Read(f,elfbase,elfhead.data(),elfhead.size());
    Put64(elfhead.data()+40,0);
    Put16(elfhead.data()+60,0);
    Put16(elfhead.data()+62,0);
    struct Segment { std::uint32_t type; std::uint64_t off; std::uint64_t size; };
    std::vector<Segment> segments;
    std::uint64_t end=phbytes;
    for(std::uint16_t i=0;i<phcount;++i) {
        const auto* p=elfhead.data()+phoff+std::size_t(i)*56;
        Segment segment{U32(p),U64(p+8),U64(p+32)};
        if(!Fits(segment.off,segment.size,MaxSize)) throw std::runtime_error("Unsupported ELF output size");
        end=std::max(end,segment.off+segment.size);
        segments.push_back(segment);
    }
    std::vector<bool> seen(phcount,false);
    struct Copy { std::uint64_t from,to,size; };
    std::vector<Copy> parts;
    std::uint64_t mappedEnd=phbytes;
    for(std::uint16_t i=0;i<count;++i) {
        const auto* p=entries.data()+std::size_t(i)*32;
        const auto props=U64(p);
        if(props&2) throw std::runtime_error("Encrypted SELF segment: provide a decrypted ELF/PRX");
        if(props&8) throw std::runtime_error("Compressed SELF segment: not supported by built-in extraction");
        if(props&(1ull<<16)) continue;
        if(!(props&(1ull<<11))) continue;
        const auto index=(props>>20)&0xffffu;
        const auto from=U64(p+8), stored=U64(p+16), unpacked=U64(p+24);
        if(index>=segments.size() || seen[index] || stored!=unpacked || stored!=segments[index].size ||
           !Fits(from,stored,declared)) throw std::runtime_error("Invalid SELF segment layout");
        seen[index]=true;
        parts.push_back({from,segments[index].off,stored});
        mappedEnd=std::max(mappedEnd,segments[index].off+stored);
    }
    if(parts.empty()) throw std::runtime_error("No usable ELF segments in SELF");
    for(std::size_t i=0;i<segments.size();++i)
        if(segments[i].type==1 && segments[i].size!=0 && !seen[i])
            throw std::runtime_error("Required ELF load segment not present in SELF");
    const auto tail=size-declared;
    if(tail>MaxTail || mappedEnd>end || tail>end-mappedEnd)
        throw std::runtime_error("Unsupported SELF trailing data");
    fs::create_directories(target.parent_path());
    std::ofstream out(target,std::ios::binary|std::ios::trunc);
    if(!out) throw std::runtime_error("Cannot create temporary ELF");
    out.write(reinterpret_cast<const char*>(elfhead.data()),static_cast<std::streamsize>(elfhead.size()));
    std::array<char,65536> chunk{};
    const auto copy=[&](std::uint64_t from,std::uint64_t to,std::uint64_t n){
        while(n) {
            const auto amount=static_cast<std::size_t>(std::min<std::uint64_t>(n,chunk.size()));
            Read(f,from,chunk.data(),amount);
            out.seekp(static_cast<std::streamoff>(to));
            out.write(chunk.data(),static_cast<std::streamsize>(amount));
            if(!out) throw std::runtime_error("Failed writing extracted ELF");
            from+=amount;to+=amount;n-=amount;
        }
    };
    for(const auto& c:parts) copy(c.from,c.to,c.size);
    if(tail) copy(declared,mappedEnd,tail);
    if(end>mappedEnd+tail) { out.seekp(static_cast<std::streamoff>(end-1));out.put('\0'); }
    out.flush();
    if(!out) throw std::runtime_error("Could not complete ELF extraction");
}
}
