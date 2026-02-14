#include <fstream>
#include <iostream>
#include <vector>

using Data = std::vector<unsigned char>;

template <typename T>
using Iterator = std::istreambuf_iterator<T>;

Data readDataFrom(const std::string& path)
{
    auto in = std::ifstream(path, std::ios::binary);

    if (!in)
        throw std::runtime_error("Error: cannot open input file: " + path);

    auto data = Data(Iterator(in), Iterator<char>());
    in.close();

    return data;
}

struct ProgramArgs
{
    std::string input;
    std::string output;
    std::string resource;
    std::string category = "Resources";
};

void run(const ProgramArgs& args)
{
    auto data = readDataFrom(args.input);

    auto out = std::ofstream(args.output);

    if (!out)
        throw std::runtime_error("Error: cannot open output file: " + args.output);

    out << "#include \"ResourceEmbedLib.h\"\n\n";
    out << "static const auto resource = Resources::Data(\"" << args.resource
        << "\", {\n";

    for (size_t i = 0; i < data.size(); ++i)
    {
        if (i % 16 == 0)
            out << "    ";

        out << static_cast<unsigned int>(data[i]);

        if (i + 1 < data.size())
            out << ",";

        if (i % 16 == 15 || i + 1 == data.size())
            out << "\n";
        else
            out << " ";
    }

    out << "}, \"" << args.category << "\");\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write output file: " + args.output);
    }
}

ProgramArgs getArgs(int argc, char* argv[])
{
    if (argc < 4 || argc > 5)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator <input_file> <output_cpp> <resource_name> [category]");
    }

    auto args = ProgramArgs();

    args.input = argv[1];
    args.output = argv[2];
    args.resource = argv[3];

    if (argc == 5)
        args.category = argv[4];

    return args;
}

int main(int argc, char* argv[])
{
    try
    {
        auto args = getArgs(argc, argv);
        run(args);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}