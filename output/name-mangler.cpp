#include <algorithm>

#include "name-mangler.h"

using namespace output;

template <typename M>
static std::vector<std::string> mapStrings(std::vector<std::string> const& in, M mapper)
{
    std::vector<std::string> result;
    std::for_each(in.begin()
                , in.end()
                , [&](std::string const& s)
                  {
                      result.push_back(mapper(s));
                  });
    return std::move(result);
}

std::string output::formName(std::string const& name)
{
    return "$c_" + name;
}

std::vector<std::string> output::formNames(std::vector<std::string> const& names)
{
    return mapStrings(names, formName);
}

std::string output::formClassName(std::string const& name)
{
    return "$cls_" + name;
}

std::string output::formSubName(std::string const& name, util::uid space_id)
{
    return "$s_" + name + '$' + space_id.str();
}

std::string output::formTransientParam(std::string const& param)
{
    return "$tp_" + param;
}

std::vector<std::string> output::formTransientParams(std::vector<std::string> const& params)
{
    return mapStrings(params, formTransientParam);
}

std::string output::formAsyncRef(util::uid const& id)
{
    return "$ar_" + id.str();
}

std::string output::formAnonymousFunc(util::uid const& id)
{
    return "$anf_" + id.str();
}

std::string const output::TERM_REGULAR_ASYNC_CALLBACK("$racb");
std::string const output::TERM_EXCEPTION("$exception");
std::string const output::TERM_CONDITIONAL_CALLBACK_PARAMETER("$ccp");
