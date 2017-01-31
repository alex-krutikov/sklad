import qbs

Application {
    name: "sklad"

    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "Qt.sql" }
    Depends { name: "Qt.printsupport" }

    cpp.cxxFlags: [
        "--no-rtti",
        "--no-exceptions",
    ]

    cpp.cxxLanguageVersion: "c++14"

    consoleApplication: false

    cpp.includePaths: [
        ".",
    ]

    files: [
        "*.h",
        "*.hpp",
        "*.c",
        "*.cpp",
        "*.qrc",
        "ui/*.ui",
    ]
}

