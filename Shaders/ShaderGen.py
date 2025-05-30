import os
import sys

args = sys.argv
path = args[1]

print(path)


def generate_header(path, name, vertex, tcs, tes, geometry, fragment):

    header_content = "#pragma once\n\n"
    header_content += f"#define SHADER_SHADERNAME_{name} \"{name}\"\n"
    header_content += f"#define LOAD_SHADER_{name} \"{name}\", {name}_vertexShader, {name}_tcsShader, {name}_tesShader, {name}_geometryShader, {name}_fragmentShader\n\n"

    with open(path + "\\" + vertex, "r") as f:
        shader_code = f.read()
    header_content += f'const char* {name}_vertexShader = R"({shader_code})";\n\n'

    
    if tcs != None:
        with open(path + "\\" + tcs, "r") as f:
            shader_code = f.read()
        header_content += f'const char* {name}_tcsShader = R"({shader_code})";\n\n'
    else:
        header_content += f'const char* {name}_tcsShader = NULL;\n\n'

    if tes != None:
        with open(path + "\\" + tes, "r") as f:
            shader_code = f.read()
        header_content += f'const char* {name}_tesShader = R"({shader_code})";\n\n'
    else:
        header_content += f'const char* {name}_tesShader = NULL;\n\n'


    if geometry != None:
        with open(path + "\\" + geometry, "r") as f:
            shader_code = f.read()
        header_content += f'const char* {name}_geometryShader = R"({shader_code})";\n\n'
    else:
        header_content += f'const char* {name}_geometryShader = NULL;\n\n'

    with open(path + "\\" + fragment, "r") as f:
        shader_code = f.read()
    header_content += f'const char* {name}_fragmentShader = R"({shader_code})";\n\n'

    # Write to `shaders.h`
    with open(path + "\\generated.h", "w") as f:
        f.write(header_content)
    return name + "\\generated.h"


generated = []

for root, _, files in os.walk(path):
    if root == path: continue
    name = root.split("\\")[-1]
    print(name)
    print(files)
    vertex, tcs, tes, geometry, fragment = None, None, None, None, None
    for file in files:
        if file.__contains__("vertex"): vertex = file
        if file.__contains__("tcs"): tcs = file
        if file.__contains__("tes"): tes = file
        if file.__contains__("geometry"): geometry = file
        if file.__contains__("fragment"): fragment = file
    if vertex == None or fragment == None: 
        print("Missing shader!")
        continue
    gen = generate_header(root, name, vertex, tcs, tes, geometry, fragment)
    generated.append(gen)

IncludeFile = path + "Shaders.h"
f = open(IncludeFile, "w")
f.write("#pragma once\n")
for gen in generated:
    f.write(f"#include \"{gen}\"\n")
f.close()
    