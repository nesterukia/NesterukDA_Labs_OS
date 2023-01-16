#include <gelf.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
uintptr_t entry_point(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open()");
        return 0;
    }
 
    if (!elf_version(EV_CURRENT)) {
        fprintf(stderr, "elf_version(): %s\n", elf_errmsg(elf_errno()));
        return 0;
    }
 
    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf) {
        fprintf(stderr, "elf_begin(): %s\n", elf_errmsg(elf_errno()));
        close(fd);
        return 0;
    }
 
    GElf_Ehdr ehdr;
    if (!gelf_getehdr(elf, &ehdr)) {
        fprintf(stderr, "gelf_getehdr(): %s\n", elf_errmsg(elf_errno()));
        elf_end(elf);
        close(fd);
        return 0;
    }
 
    elf_end(elf);
    close(fd);
 
    return ehdr.e_entry;
}
 
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
 
    uintptr_t entry = entry_point(argv[1]);
    if (!entry) {
        return EXIT_FAILURE;
    }
 
    printf("Entry: %" PRIxPTR "\n", entry);
 
    return EXIT_SUCCESS;
}