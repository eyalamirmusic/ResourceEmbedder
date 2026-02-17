#include <fstream>
#include <iostream>
#include <sstream>
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

std::string getDirectory(const std::string& path)
{
    auto pos = path.find_last_of("/\\");

    if (pos != std::string::npos)
        return path.substr(0, pos);

    return ".";
}

std::string getFilename(const std::string& path)
{
    auto pos = path.find_last_of("/\\");

    if (pos != std::string::npos)
        return path.substr(pos + 1);

    return path;
}

std::vector<std::string> splitComma(const std::string& input)
{
    auto result = std::vector<std::string>();
    auto stream = std::istringstream(input);
    auto token = std::string();

    while (std::getline(stream, token, ','))
    {
        if (!token.empty())
            result.push_back(token);
    }

    return result;
}

struct EmbedArgs
{
    std::string cppOutput;
    std::string category;
    std::vector<std::string> inputFiles;
};

struct InitArgs
{
    std::string headerOutput;
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

void writeEntriesCpp(const EmbedArgs& args)
{
    auto out = std::ofstream(args.cppOutput);

    if (!out)
        throw std::runtime_error(
            "Error: cannot open output file: " + args.cppOutput);

    out << "#include \"ResourceEmbedLib.h\"\n\n";

    for (size_t i = 0; i < args.inputFiles.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        out << "extern const unsigned char " << varPrefix << "_data[];\n";
        out << "extern const unsigned long " << varPrefix << "_size;\n";
    }

    out << "\nnamespace Resources\n";
    out << "{\n";
    out << "const Entries& getResourceEntries()\n";
    out << "{\n";
    out << "    static const Entries entries = {\n";

    for (size_t i = 0; i < args.inputFiles.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        auto resourceName = getFilename(args.inputFiles[i]);
        out << "        {" << varPrefix << "_data, " << varPrefix << "_size, \""
            << resourceName << "\", \""
            << args.category << "\"}";

        if (i + 1 < args.inputFiles.size())
            out << ",";

        out << "\n";
    }

    out << "    };\n\n";
    out << "    return entries;\n";
    out << "}\n";
    out << "}\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write output file: " + args.cppOutput);
    }
}

void writeInitHeader(const InitArgs& args)
{
    auto out = std::ofstream(args.headerOutput);

    if (!out)
        throw std::runtime_error(
            "Error: cannot open header file: " + args.headerOutput);

    out << "#pragma once\n\n";
    out << "#include \"ResourceEmbedLib.h\"\n\n";
    out << "namespace Resources\n";
    out << "{\n";
    out << "const Entries& getResourceEntries();\n";
    out << "static const Initializer resourceInitializer "
        << "{getResourceEntries()};\n";
    out << "}\n";

    if (!out)
    {
        throw std::runtime_error(
            "Error: failed to write header file: " + args.headerOutput);
    }
}

EmbedArgs getEmbedArgs(int argc, char* argv[])
{
    if (argc != 3)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator embed <cpp_output> <category> "
            "<file1,file2,...>");
    }

    auto args = EmbedArgs();

    args.cppOutput = argv[0];
    args.category = argv[1];
    args.inputFiles = splitComma(argv[2]);

    if (args.inputFiles.empty())
        throw std::runtime_error("Error: no input files specified");

    return args;
}

InitArgs getInitArgs(int argc, char* argv[])
{
    if (argc != 1)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator init <header_output>");
    }

    return {argv[0]};
}

void writeResources(const EmbedArgs& args)
{
    auto outputDir = getDirectory(args.cppOutput);

    for (size_t i = 0; i < args.inputFiles.size(); ++i)
    {
        auto varPrefix = "resource_" + std::to_string(i);
        auto outputC = outputDir + "/BinaryResource"
                       + std::to_string(i) + ".c";
        writeDataFile(args.inputFiles[i], outputC, varPrefix);
    }
}

void runEmbed(const EmbedArgs& args)
{
    writeResources(args);
    writeEntriesCpp(args);
}

std::string parseCommand(int argc, char* argv[])
{
    if (argc < 2)
    {
        throw std::runtime_error(
            "Usage: ResourceGenerator <embed|init> ...");
    }

    return {argv[1]};
}

void run(int argc, char* argv[])
{
    auto command = parseCommand(argc, argv);

    if (command == "embed")
    {
        auto args = getEmbedArgs(argc - 2, argv + 2);
        runEmbed(args);
    }
    else if (command == "init")
    {
        auto args = getInitArgs(argc - 2, argv + 2);
        writeInitHeader(args);
    }
    else
    {
        throw std::runtime_error(
            "Unknown command: " + command +
            "\nUsage: ResourceGenerator <embed|init> ...");
    }
}

int main(int argc, char* argv[])
{
    try
    {
        run(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}