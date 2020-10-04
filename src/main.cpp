#include "debug.h"
#include "error.h"
#include "generator.h"
#include "lexer.h"
#include "parser.h"
#include "quad_generator.h"

#include <assert.h>
#include <sstream>

#ifdef PRINT_OUTPUT
std::ofstream outstream;
#endif  // PRINT_OUTPUT

#ifdef PRINT_ERROR
std::ofstream errstream;
extern int error_flag;
#endif  // PRINT_ERROR

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
    if (argc <= 1) {
        Error("no input files");
        exit(0);
    }
    auto srcfile = read_source_file(argv[1]);
#ifdef PRINT_OUTPUT
    outstream.open("output.txt");
    assert(outstream.is_open());
#endif  // PRINT_OUTPUT

#ifdef PRINT_ERROR
    errstream.open("error.txt");
    assert(errstream.is_open());
#endif  // PRINT_ERROR

    Lexer lexer(srcfile, argv[1]);
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
