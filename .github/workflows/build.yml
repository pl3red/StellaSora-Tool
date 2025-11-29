name: Build StellaSora DLL

on:
  push:
    branches: ["main"]
  pull_request:
    branches: ["main"]

jobs:
  build:
    runs-on: windows-latest

    steps:
    # Checkout repo
    - name: Checkout
      uses: actions/checkout@v4

    # Install MSBuild
    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v2

    # Setup NuGet (required by Visual Studio solutions)
    - name: Setup NuGet
      uses: NuGet/setup-nuget@v2

    # Restore packages (even if none is needed)
    - name: Restore NuGet packages
      run: nuget restore StellaSora-Tool.sln

    # Build the solution in Release | x64
    - name: Build DLL (Release x64)
      run: msbuild StellaSora-Tool.sln /p:Configuration=Release /p:Platform=x64 /m

    # List all files, exactly like: dir /a-d /b /s
    - name: List all files (debugging)
      shell: cmd
      run: |
        echo === DIRECTORY LIST (dir /a-d /b /s) ===
        dir /a-d /b /s

    # Find DLL, LIB, PDB output automatically
    - name: Show build outputs
      shell: cmd
      run: |
        echo === DLL Files ===
        dir /s /b *.dll
        echo.
        echo === PDB Files ===
        dir /s /b *.pdb
        echo.
        echo === LIB Files ===
        dir /s /b *.lib

    # Upload all DLL artifacts
    - name: Upload built artifacts
      uses: actions/upload-artifact@v4
      with:
        name: StellaSora-Release-x64
        path: |
          **\*.dll
          **\*.pdb
          **\*.lib
        retention-days: 30
