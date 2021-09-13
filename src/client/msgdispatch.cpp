#include "../../header/client/msgdispatch.h"

std::string_view as_str(const CheckResult sv)
{
    static constexpr Map<CheckResult, std::string_view, kerr_map.size()> 
        map = {{kerr_map}};
    return map.at(sv);
}