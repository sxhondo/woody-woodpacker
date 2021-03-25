#include "woody-woodpacker.h"

int
w_atoi(const char *str)
{
   unsigned long  nb;
   int            sign;
   size_t         i;

   sign = 1;
   nb = 0;
   i = 0;
   while (str[i] == ' ' || str[i] == '\n' || str[i] == '\v'
         || str[i] == '\f' || str[i] == '\r' || str[i] == '\t') i++;
   if (str[i] == '-' || str[i] == '+')
      sign = (str[i++] == '-') ? -1 : 1;
   while (str[i])
   {
      if (str[i] < '0' || str[i] > '9') break ;
      nb = nb * 10 + (str[i++] - '0');
   }
   return ((int)(nb * sign));
}

static uint32_t
parse_args(char **argv, uint32_t key[])
{

}

int
main(int argc, char **argv)
{
   uint32_t key[4];
   void     *target, *payload;
   int      tsize, psize;

   if (argc != 3)
   {
      fprintf(stderr, "usage:\n  %s elf_file payload\n", argv[0]);
      exit(1);
   }

   int tfd = map_target(argv[1], &target, &tsize);
   int pfd = map_payload(argv[2], &payload, &psize);

   parse_args(argv, &key);
   inject_payload(target, payload, tsize, psize);

   munmap(&target, tsize);
   munmap(&payload, psize);
   close(pfd);
   close(tfd);
   exit(0);
}