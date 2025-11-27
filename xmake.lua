add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {
  outputdir = "./"
})

function project_module(name, deps)
  add_rules("mode.debug", "mode.release")

  target(name)
    set_kind("shared")
    add_includedirs("include", { public = true })
    add_includedirs("src")
    add_deps(unpack(deps))
    add_files("src/**.c")
    project_config() 
end

function project_config() 
  set_warnings("all", "error")
  if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
  end
end

includes(
  "modules/core",
  "modules/lexer",
  "modules/parser",
  "modules/sema",
  "modules/hir",
  "modules/lir"
)

target("lou")
  set_kind("binary")
  add_includedirs("src")
  add_files("src/**.c")
  add_deps("louCore", "louLexer", "louParser", "louSema", "louHir", "louLir")
  project_config() 

