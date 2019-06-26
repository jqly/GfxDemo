#include "utility.h"

#include <fstream>
#include <cassert>
#include "calc.h"

namespace util
{

std::string format(const std::string& fmt)
{
    return fmt;
}

void quat_ostream(std::ostream& os, const calc::Quat& q)
{
    os << q.w << "+" << q.x << "i+" << q.y << "j+" 
        << q.z << "k";
}

std::ostream& operator<<(std::ostream& os, const calc::Quat& q)
{
    quat_ostream(os, q);
    return os;
}

std::string read_file(const std::string& path)
{
	std::ifstream fin(path);

	assert(!fin.fail());

	fin.ignore(std::numeric_limits<std::streamsize>::max());
	auto size = fin.gcount();
	fin.clear();

	fin.seekg(0, std::ios_base::beg);
	auto source = std::unique_ptr<char>(new char[size]);
	fin.read(source.get(), size);

	return std::string(source.get(), static_cast<std::string::size_type>(size));
}

std::string get_file_base_dir(const std::string& filename)
{
    auto probe=filename.find_last_of("/\\");
    if (probe == std::string::npos)
        return "";
    return filename.substr(0,probe);
}

std::string get_file_extension(const std::string& filename)
{
	auto found = filename.find_last_of(".");
    if (found == std::string::npos) {
        std::cerr << "Wrong input file.\n";
        exit(1);
    }
    return filename.substr(found);
}

}