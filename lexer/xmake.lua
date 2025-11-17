add_rules("mode.debug", "mode.release")
add_requires("catch2 3.x")

target("lexer")
  set_kind("shared")
  set_languages("c++23")
  add_deps("logger")
  add_includedirs("include", {
    public = true
  })
  add_includedirs("src")
  add_files("src/*.cpp")

target("lexer-tests")
  add_tests("tests")
  add_packages("catch2")
  add_deps("lexer")
  add_files("tests/*.cpp")
  set_languages("c++20")
