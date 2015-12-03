from django.shortcuts import render
from django.http import HttpResponse


def archivos(request):
    return render(request, "files/dashboard.html", {})

def agrega_archivo(request):
    return HttpResponse("En agrega archivos")

def elimina_archivo(request, nombre_archivo):
    msg = "Elimina Archivo " + nombre_archivo
    return HttpResponse(msg)

def extraer_path(request, nombre_archivo):
    msg = "Extraer Path de" + nombre_archivo
    return render(request, 'files/input_path.html', {})

def extraer_archivo(request):
    return HttpResponse("Extraer archivo")

def set_cache(request):
    return HttpResponse("En cache")
