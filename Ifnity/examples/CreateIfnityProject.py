import os
import shutil

def crear_proyecto_desde_base(nuevo_nombre_proyecto):
    # Ruta donde está ubicado el script
    ruta_base = os.path.dirname(os.path.abspath(__file__))
    
    # Ruta de la carpeta base (BaseProject)
    ruta_origen = os.path.join(ruta_base, "EmptyProject")
    
    # Ruta de la nueva carpeta (destino)
    ruta_destino = os.path.join(ruta_base, nuevo_nombre_proyecto)
    
    try:
        # Verificar si la carpeta base existe
        if not os.path.exists(ruta_origen):
            raise FileNotFoundError(f"La carpeta base 'EmptyProject' no existe en '{ruta_base}'.")
        
        # Copiar la carpeta base al destino con el nuevo nombre
        if os.path.exists(ruta_destino):
            print(f"La carpeta '{ruta_destino}' ya existe. Se sobreescribirá.")
            shutil.rmtree(ruta_destino)
        shutil.copytree(ruta_origen, ruta_destino)
        print(f"Carpeta base copiada a '{ruta_destino}'.")
        
        # Ruta al archivo CMakeLists.txt en la nueva carpeta
        ruta_cmake = os.path.join(ruta_destino, "CMakeLists.txt")
        
        # Verificar si existe el archivo CMakeLists.txt en la carpeta base
        if not os.path.exists(ruta_cmake):
            raise FileNotFoundError(f"No se encontró el archivo CMakeLists.txt en '{ruta_destino}'.")
        
        # Leer y modificar el archivo CMakeLists.txt
        with open(ruta_cmake, "r") as archivo:
            contenido = archivo.read()
        
        # Reemplazar el nombre del proyecto en el archivo
        contenido_modificado = f"""cmake_minimum_required(VERSION 3.8)

# Nombre del proyecto
project({nuevo_nombre_proyecto})
""" + "\n".join(contenido.split("\n")[3:])
        
        # Guardar los cambios en el archivo
        with open(ruta_cmake, "w") as archivo:
            archivo.write(contenido_modificado)
        print(f"Archivo CMakeLists.txt actualizado con el nombre del proyecto '{nuevo_nombre_proyecto}'.")
        
    except Exception as e:
        print(f"Error: {e}")

# Solicitar el nombre del proyecto al usuario
nombre_proyecto = input("Introduce el nombre del nuevo proyecto: ")
crear_proyecto_desde_base(nombre_proyecto)
