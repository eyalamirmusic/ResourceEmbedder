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

struct ResourceTriplet
{
    std::string input;
    std::string outputC;
    std::string resourceName;
};

struct ProgramArgs
{
    std::string headerOutput;
    std::string category;
    std::vector<ResourceTriplet> resources;
};

void writeDataFile(const std::string& input,
                   const std::string& output,
                   const std::string& varPrefix)
{
    auto data = readDataFrom(input);

    auto out = std::ofstream(output);

    if (!out)
        throw std::runtime_error("Error: cannot open output file: " + output);

    out << "const unsigned char " << varPrefix << "_data[] = {\n";

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

    out << "};\n\n";
    out << "const unsigned long " << varPrefix
        << "_size = sizeof(" << varPrefix << "_data);\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write output file: " + output);
    }
}

void writeHeader(const ProgramArgs& args)
{
    auto out = std::ofstream(args.headerOutput);

    if (!out)
        throw std::runtime_error(
            "Error: cannot open header file: " + args.headerOutput);

    out << "#pragma once\n\n";
    out << "#include \"ResourceEmbedLib.h\"\n\n";

    for (size_t i = 0; i < args.resources.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        out << "extern const unsigned char " << varPrefix << "_data[];\n";
        out << "extern const unsigned long " << varPrefix << "_size;\n";
    }

    out << "\nnamespace Resources\n";
    out << "{\n";
    out << "static const Entries resource_entries = {\n";

    for (size_t i = 0; i < args.resources.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        out << "    {" << varPrefix << "_data, " << varPrefix << "_size, \""
            << args.resources[i].resourceName << "\", \""
            << args.category << "\"}";

        if (i + 1 < args.resources.size())
            out << ",";

        out << "\n";
    }

    out << "};\n\n";
    out << "static const Initializer resourceInitializer {resource_entries};\n";
    out << "}\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write header file: " + args.headerOutput);
    }
}

ProgramArgs getArgs(int argc, char* argv[])
{
    if (argc < 3 || (argc - 3) % 3 != 0)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator <header_output> <category> "
            "[<input> <output.c> <resource_name>]...");
    }

    auto args = ProgramArgs();

    args.headerOutput = argv[1];
    args.category = argv[2];

    for (int i = 3; i < argc; i += 3)
    {
        args.resources.push_back({argv[i], argv[i + 1], argv[i + 2]});
    }

    return args;
}

int main(int argc, char* argv[])
{
    try
    {
        auto args = getArgs(argc, argv);

        for (size_t i = 0; i < args.resources.size(); ++i)
        {
            auto varPrefix = "resource_" + std::to_string(i);
            writeDataFile(args.resources[i].input,
                          args.resources[i].outputC,
                          varPrefix);
        }

        writeHeader(args);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}