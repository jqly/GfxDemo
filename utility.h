#ifndef UTILITY_H
#define UTILITY_H


#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include "calc.h"

namespace util
{

std::string format(const std::string& fmt);

template<typename First, typename ... Others>
std::string format(
    const std::string& fmt, 
    First first, 
    Others ... others)
{
    auto is_op_delim = [&fmt](int idx){
        if (fmt[idx]=='{') 
            return true;
        return false;
    };

    auto is_ed_delim = [&fmt](int idx){
        if (fmt[idx]=='}') 
            return true;
        return false;
    };

    auto is_delim_escape = [&fmt](int idx) {
        if (idx+1>=fmt.size()) return false;
        if ((fmt[idx]=='{' && fmt[idx+1]=='{') || 
			    (fmt[idx]=='}' && fmt[idx+1]=='}'))
            return true;
        return false;
    };

    std::string first_part, format_part;
    int state = 0, first_part_ed = 0;
    for (int idx = 0; idx < fmt.size(); ++idx) {
        if (is_delim_escape(idx)) {
            if (state == 0)
                first_part += fmt[idx+1];
            else if (state == 1)
                format_part += fmt[idx+1];
            idx++;
        }
        else if (is_op_delim(idx)) {
            if (state == 0)
                state = 1;
            else
                throw std::invalid_argument("Delimiter mismatch");
        }
        else if (is_ed_delim(idx)) {
            if (state == 1) {
                state = 2;
                first_part_ed = idx+1;
            }
            else
                throw std::invalid_argument("Delimiter mismatch");
            break;
        }
        else {
            if (state == 0)
                first_part += fmt[idx];
            else if (state == 1)
                format_part += fmt[idx];
        }
    }
    if (state != 2)
        throw std::invalid_argument("Delimiter mismatch");
    auto remaining_part = fmt.substr(
        first_part_ed, fmt.size() - first_part_ed);
    std::stringstream ss;

	if (std::is_floating_point<First>::value || 
		    calc::is_Mat<First>::value || 
			std::is_same<calc::Quat,First>::value) {
		// Setting precision of a floating point number.
		// Changing width of the integral part: Unsupported.
		auto pos = format_part.find('.');
		if (pos != std::string::npos && pos < format_part.size()-1) {
            auto curr_prec = std::stoi(format_part.substr(pos+1));
			auto prev_prec = ss.precision(curr_prec);
            ss << first;
			ss.precision(prev_prec);
		}
		else
			ss << first;
	}
	else
		ss << first;
	
    return first_part + ss.str() + format(remaining_part, others...);
}

template<typename ...Args>
void print(std::ostream& out=std::cout, Args... args)
{
	out << format(args...);
}

template<typename ValType, int NumRows, int NumCols>
void matrix_ostream(
    std::ostream& os,
	const calc::Mat<ValType, NumRows, NumCols>& a,
	std::string ldelim="{", std::string rdelim="}", 
	std::string val_sep=",", std::string vec_sep=",")
{
	auto p = begin(a);
	os << ldelim;
	for (int row = 0; row < NumRows; ++row) {
		if (NumCols != 1)
			os << ldelim;
		for (int col = 0; col < NumCols; ++col) {
			os << p[col*NumRows+row];
			if (col != NumCols-1)
				os << val_sep;
		}
		if (NumCols != 1)
			os << rdelim;
		if (row!=NumRows-1)
			os << vec_sep;
	}
	os << rdelim;
}

void quat_ostream(std::ostream&, const calc::Quat&);

template<typename ValType, int NumRows, int NumCols>
std::ostream& operator<<(
    std::ostream& os, const calc::Mat<ValType, NumRows, NumCols>& a)
{
    matrix_ostream(os, a);
    return os;
}

std::ostream& operator<<(std::ostream&, const calc::Quat&);

std::string read_file(const std::string& path);

// No trailling dirsep.
std::string get_file_base_dir(const std::string& filename);

std::string get_file_extension(const std::string& filename);

}

#endif
