add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {
  outputdir = "./"
})

function project_config() 
  set_warnings("all", "error")
  if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
  end
end

includes("core", "lexer", "parser")

target("lou")
  set_kind("binary")
  add_includedirs("src")
  add_files("src/*.c")
  add_deps("louCore", "louLexer", "louParser")
  project_config() 

