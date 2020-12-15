#include<elf.h>
#include<link.h>
#include<stddef.h>
#include<stdint.h>

uint32_t gnu_hash(const uint8_t *name)
{
  uint32_t h = 5381;
  for (; *name; ++name){
    h = (h << 5) + h + *name;
  }
  return h;
}

int strcmp(const char *s1, const char *s2)
{
  int i = 0;
  for(; s1[i] && s1[i] == s2[i]; ++i);
  return s1[i] - s2[i];
}

void callPrintfFromSym(ElfW(Sym) *symTab, const uintptr_t l_addr) {
  int (*ptr)() = (void *)symTab->st_value + l_addr;
  ptr("Hello World!\n");
}

int findAndCallPrintf(const char *strTab, ElfW(Sym) *symTab,
    const uintptr_t l_addr)
{
  for (; (uintptr_t)symTab < (uintptr_t)strTab; ++symTab)
  {
    if (!symTab->st_shndx)
      continue;

    if (!strcmp("printf", strTab + symTab->st_name))
    {
      callPrintfFromSym(symTab, l_addr);
      return 0;
    }
  }
  return 1;
}

int linkMapToSymTab(struct link_map *map)
{
  ElfW(Dyn) *d = map->l_ld;
  ElfW(Dyn) *strTab;
  ElfW(Dyn) *symTab;
  ElfW(Dyn) *hash;
  uint32_t hashValue = gnu_hash("printf");

  for (; d->d_tag != DT_NULL; ++d)
  {
    if (d->d_tag == DT_STRTAB)
      strTab = d;
    else if (d->d_tag == DT_SYMTAB)
      symTab = d;
    else if (d->d_tag == DT_GNU_HASH)
      hash = (void*)d;
  }


  void *hashTabAddr = (uint32_t*)hash->d_un.d_ptr;
  if ((uintptr_t)hashTabAddr < (uintptr_t)map->l_addr)
    hashTabAddr += map->l_addr;
  void *strTabAddr = (void*)strTab->d_un.d_ptr;
  if ((uintptr_t)strTabAddr < (uintptr_t)map->l_addr)
    strTabAddr += map->l_addr;
  void *symTabAddr = (void *)symTab->d_un.d_ptr;
  if ((uintptr_t)symTabAddr < (uintptr_t)map->l_addr)
    symTabAddr += map->l_addr;

  uint32_t *hashTab = hashTabAddr;
  ElfW(Sym) *symbTab = symTabAddr;
  char* stringTab = strTabAddr;

  uint32_t nbuckets = hashTab[0];
  uint32_t symOffset = hashTab[1];
  uint32_t bloomSize = hashTab[2];
  uint32_t bloomShift = hashTab[3];
  size_t *bloom = (void*)&hashTab[4];
  uint32_t *buckets = (void*)&bloom[bloomSize];
  uint32_t *chain = &buckets[nbuckets];


  size_t word = bloom[(hashValue / __ELF_NATIVE_CLASS) % bloomSize];
  size_t mask = 0
    | (size_t) 1 << (hashValue % __ELF_NATIVE_CLASS)
    | (size_t) 1 << ((hashValue >> bloomShift) % __ELF_NATIVE_CLASS);

  if ((word & mask) != mask)
    return 1;

  uint32_t symix = buckets[hashValue % nbuckets];
  if (symix < symOffset)
    return 1;
  
  while (1) {
    if (!symbTab[symix].st_shndx)
      continue;
    const char* symName = stringTab + symbTab[symix].st_name;
    const uint32_t h = chain[symix - symOffset];

    if (hashValue|1 == h|1 && !strcmp("printf", symName)) {
      callPrintfFromSym(&symbTab[symix], map->l_addr);
      return 0;
    }
    if (h & 1)
      break;

    symix++;
  }
  return 1;
}

int iterateLinkMap(struct r_debug *d)
{
  struct link_map *li_map = d->r_map;
  for(; li_map; li_map = li_map->l_next)
  {
    if (!linkMapToSymTab(li_map))
      return 0;
  }
  return 1;
}

struct r_debug* auxvToR_Debug(ElfW(auxv_t) *auxv)
{
  ElfW(Phdr) *phdrIter;
  ElfW(Phdr) *phdrBase;
  size_t phNum;

  for (; auxv->a_type != AT_NULL; ++auxv)
  {
    if (auxv->a_type == AT_PHDR)
    {
      phdrIter = (void *)auxv->a_un.a_val;
      phdrBase = (void *)auxv->a_un.a_val;
    }
    else if (auxv->a_type == AT_PHNUM)
      phNum = (size_t)auxv->a_un.a_val;
  }

  for (; phdrIter < phdrBase + phNum && phdrIter->p_type != PT_DYNAMIC;
      ++phdrIter);
  if (phdrIter >= phdrBase + phNum)
    return NULL;

  ElfW(Dyn) *dynamic = (void *)((uintptr_t)phdrBase + (uintptr_t)phdrIter->p_vaddr);
  struct r_debug * debug = NULL;

  for (; dynamic->d_tag != DT_NULL; ++dynamic)
  {
    if (dynamic->d_tag == DT_DEBUG)
    {
      debug = (void *)dynamic->d_un.d_ptr;
      break;
    }

  }

  return debug;
}

int main(int argc, char *argv[], char *envp[]) {
  char **ptr = envp;
  for (; ptr[0]; ++ptr);
  ++ptr;
  ElfW(auxv_t) *auxv = (void*)ptr;

  struct r_debug *deb = auxvToR_Debug(auxv);
  if (!deb)
    return 1;
  return iterateLinkMap(deb);
}
