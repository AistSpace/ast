# scripts/doxygen

根目录 `doxyfile` 引用的 Doxygen HTML 定制文件：

| 文件 | doxyfile 选项 | 用途 |
| --- | --- | --- |
| `layout.xml` | `LAYOUT_FILE` | 导航栏布局 |
| `style.css` | `HTML_EXTRA_STYLESHEET` | 自定义样式 |
| `footer.html` | `HTML_FOOTER` | 页脚，含 Vercount 访问量脚本 |
| `header.html` | `HTML_HEADER`（当前未启用） | 自定义页头 |

## footer.html 与 Doxygen 版本的差异

`footer.html` 来自 Doxygen 1.10.0 生成的默认模板：

```bash
doxygen -w html header.html footer.html stylesheet.css
```

官方模板在仓库中的位置（1.10 起）：

```
https://raw.githubusercontent.com/doxygen/doxygen/<tag>/templates/html/footer.html
```

`!GENERATE_TREEVIEW` 分支在 **1.11.0** 起多了一行闭合标签：

| 版本 | `!GENERATE_TREEVIEW` 分支结尾 |
| --- | --- |
| 1.10.0 | `</small></address>` |
| 1.11.0 及以后 | `</small></address>` + `</div><!-- doc-content -->` |

对比结果：

- `1.10.0` → `1.11.0` 的 diff 只有这一行；
- `1.11.0` 与 `1.12.0` 的 `footer.html` 完全相同。

当前 `doxyfile` 是 `GENERATE_TREEVIEW = YES`，Doxygen 会删除整个 `!GENERATE_TREEVIEW` 块，所以这行差异不影响产物。

如果以后把 `GENERATE_TREEVIEW` 改成 `NO`，并且 Doxygen 版本 ≥ 1.11，需要在 `</small></address>` 后补上：

```html
</div><!-- doc-content -->
```

否则页面会少一个闭合 `</div>`。注意 `mattnotmitt/doxygen-action` 是在运行时通过 `apk add doxygen` 安装的（基础镜像 `alpine:edge`），实际版本不固定；更稳妥的做法是用 CI 实际使用的版本重新生成 footer。

## Vercount 访问量统计

`footer.html` 末尾引入 `https://events.vercount.one/js`，API 文档页面只上报 PV，不显示数字。`/ast/api/` 下的页面与 `docs/.vuepress` 站点使用同一个 Vercount 站点。
