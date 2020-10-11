#include "debug.h"
#include "error.h"
#include "generator.h"
#include "lexer.h"
#include "parser.h"
#include "quad_generator.h"

#include <assert.h>
#include <getopt.h>
#include <sstream>

#ifdef PRINT_OUTPUT
std::ofstream outstream;
#endif  // PRINT_OUTPUT

#ifdef PRINT_ERROR
std::ofstream errstream;
extern int error_flag;
#endif  // PRINT_ERROR

const char *output_file;
const char *source_file;

static void usage(const char *argv0)
{
  fprintf(stdout,
          "Usage: %s [options] file...\n"
          " %s compiles C0(C-like) language source file into MIPS assembly "
          "code.\n"
          "Options:\n"
          " -c <file>, Compile source <file>.\n",
          " -o <file>, Place the output into <file>.\n",
          " -g, Generate debug information.\n", argv0, argv0);
}

static int parse_opt(int argc, char *const argv[])
{
  while (1) {
    static const struct option long_options[] = {
      {.name = "compile", .has_arg = 1, .flag = nullptr, .val = 'c'},
      {.name = "output", .has_arg = 1, .flag = nullptr, .val = 'o'},
      {.name = "g", .has_arg = 0, .flag = nullptr, .val = 'g'},
    };
    int c = getopt_long(argc, argv, "co:g", long_options, NULL);
    if (c == -1)
      break;
    switch (c) {
    case 'c':
      source_file = optarg;
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'g':
      /* TODO: Add -g support */
      break;
    default:
      usage(argv[0]);
      return 1;
    }
  }
  if (!source_file) {
    Error("Must specify source file.");
    return -1;
  }
  return 0;
}

/**
 * Must return a *pointer* to the src on the heap.
 * Otherwise, it could lead to memory leak.
 */
static std::string *read_source_file(const char *filename)
{
  std::ifstream infile(filename, std::ios::in);
  std::ostringstream buf;

  if (!infile.is_open()) {
    Error("no such file or directory: \'%s\'", filename);
    return nullptr;
  }
  buf << infile.rdbuf();
  infile.close();

  return new std::string(buf.str());
}

int main(int argc, char **argv)
{
  int rc;

  rc = parse_opt(argc, argv);
  if (rc) {
    Error("Option parse failed.");
    exit(0);
  }

  auto srcfile = read_source_file(source_file);
#ifdef PRINT_OUTPUT
  outstream.open(output_file);
  assert(outstream.is_open());
#endif  // PRINT_OUTPUT

#ifdef PRINT_ERROR
  errstream.open("error.txt");
  assert(errstream.is_open());
#endif  // PRINT_ERROR

  Lexer lexer(srcfile, source_file);
  Parser parser(&lexer);
  parser.Analyse();

  if (error_flag) {
    return 0;
  }
  auto quads_fp = fopen("quads.txt", "w");

  QuadGenerator qg(&parser, quads_fp);
  qg.Gen();
  fclose(quads_fp);

  debug("**************** Code Gen ****************\n");

  auto mips_fp = fopen("mips.txt", "w");
  CodeGenerator generator(&parser, &qg, mips_fp);
  generator.Gen();
  fclose(mips_fp);

#ifdef PRINT_ERROR
  errstream.close();
#endif  // PRINT_ERROR

#ifdef PRINT_OUTPUT
  outstream.close();
#endif  // PRINT_OUTPUT

  return 0;
}
