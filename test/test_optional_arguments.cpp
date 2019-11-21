#include <doctest.hpp>
#include <argparse.hpp>

DOCTEST_TEST_CASE("Parse toggle arguments with default value [optional_arguments]") {
  argparse::ArgumentParser program("test");
  program.add_argument("--verbose", "-v")
    .default_value(false)
    .implicit_value(true);

  program.parse_args({ "./test.exe" });
  REQUIRE(program.get<bool>("--verbose") == false);
  REQUIRE(program["--verbose"] == false);
}

DOCTEST_TEST_CASE("Argument '-' is not an optional argument [optional_arguments]") {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  program.parse_args({ "./test.exe", "-"});
  REQUIRE(program.get<std::string>("input") == "-");
}

DOCTEST_TEST_CASE("Parse toggle arguments with implicit value [optional_arguments]") {
  argparse::ArgumentParser program("test");
  program.add_argument("--verbose")
    .default_value(false)
    .implicit_value(true);

  program.parse_args({ "./test.exe", "--verbose" });
  REQUIRE(program.get<bool>("--verbose") == true);
  REQUIRE(program["--verbose"] == true);
  REQUIRE(program["--verbose"] != false);
}

DOCTEST_TEST_CASE("Parse multiple toggle arguments with implicit values [optional_arguments]") {
  argparse::ArgumentParser program("test");
  program.add_argument("-a")
    .default_value(false)
    .implicit_value(true);

  program.add_argument("-u")
    .default_value(false)
    .implicit_value(true);

  program.add_argument("-x")
    .default_value(false)
    .implicit_value(true);

  program.parse_args({ "./test.exe", "-a", "-x" });
  REQUIRE(program.get<bool>("-a") == true);
  REQUIRE(program.get<bool>("-u") == false);
  REQUIRE(program.get<bool>("-x") == true);
}

DOCTEST_TEST_CASE("Parse optional arguments of many values [optional_arguments]") {
  GIVEN("a program that accepts an optional argument of many values") {
    argparse::ArgumentParser program("test");
    program.add_argument("-i").remaining().action(
        [](const std::string &value) { return std::stoi(value); });

    WHEN("provided no argument") {
      THEN("the program accepts it but gets nothing") {
        REQUIRE_NOTHROW(program.parse_args({"test"}));
        REQUIRE_THROWS_AS(program.get<std::vector<int>>("-i"),
                          std::logic_error);
      }
    }

    WHEN("provided remaining arguments follow the option") {
      program.parse_args({"test", "-i", "-42", "8", "100", "300"});

      THEN("the optional parameter consumes all of them") {
        auto inputs = program.get<std::vector<int>>("-i");
        REQUIRE(inputs.size() == 4);
        REQUIRE(inputs[0] == -42);
        REQUIRE(inputs[1] == 8);
        REQUIRE(inputs[2] == 100);
        REQUIRE(inputs[3] == 300);
      }
    }
  }
}

DOCTEST_TEST_CASE("Parse arguments of different types [optional_arguments]") {
  using namespace std::literals;

  argparse::ArgumentParser program("test");
  program.add_argument("--this-argument-is-longer-than-any-sso-buffer-that-"
                       "makes-sense-unless-your-cache-line-is-this-long"s);

  REQUIRE_NOTHROW(program.parse_args({"test"}));

  program.add_argument("-string"s, "-string-view"sv, "-builtin")
      .default_value(false)
      .implicit_value(true);

  program.parse_args({"test", "-string-view"});
  REQUIRE(program["-string"sv] == true);
  REQUIRE(program["-string-view"] == true);
  REQUIRE(program["-builtin"s] == true);
}
