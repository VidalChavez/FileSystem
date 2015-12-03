from django.shortcuts import render, redirect
from django.http import HttpResponse


def archivos(request):
    return render(request, "files/dashboard.html", {})

def agrega_archivo(request):
    return HttpResponse("En agrega archivos")

def elimina_archivo(request, nombre_archivo):
    msg = "Elimina Archivo " + nombre_archivo
    print msg
    print "xxxxxxxxxxxxxxx"
    return redirect('/')

def extraer_path(request, nombre_archivo):
    msg = "Extraer Path de" + nombre_archivo
    print msg
    print "*******************"
    return redirect('/')

def extraer_archivo(request):
    return HttpResponse("Extraer archivo")

def set_cache(request):
    return HttpResponse("En cache")
