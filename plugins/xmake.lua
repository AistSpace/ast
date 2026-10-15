includes("*/xmake.lua")

if is_plat("windows") and is_mode("debug") then
    set_suffixname("_d") -- windows的调试库使用_d后缀
end