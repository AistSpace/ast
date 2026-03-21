rule("ast")
    on_load(function (target)
        if target:plat() == "wasm" then

            -- import("core.cache.localcache")
            -- local preloadfiles = localcache.get("rule.ast", "wasm.preloadfiles")
            -- if not preloadfiles then
            --     local files = os.files("data/**|**plneph.405|**leDE1900.421|**leDE1941.405|**leDE18002100.424")
            --     preloadfiles = {}
            --     for _, file in ipairs(files) do
            --         table.insert(preloadfiles, file .. "@" .. file)
            --     end
            --     localcache.set("rule.ast", "wasm.preloadfiles", preloadfiles)
            --     localcache.save()
            -- end
            -- 添加wasm预加载文件
            target:set("values", "wasm.preloadfiles", "data")
        end
        local include_dir = path.join(os.scriptdir(), "include", target:name())
        if os.isdir(include_dir) then
            target:add("includedirs", include_dir)
        end
    end)