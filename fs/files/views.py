from django.shortcuts import render
from django.http import HttpResponse


def archivos(request):
    return render(request, "files/dashboard.html", {})

def agrega_archivo(request):
    return HttpResponse("En agrega archivos")

def elimina_archivo(request):
    return HttpResponse("En elimina Archivo")

def extraer_archivo(request):
    return HttpResponse("Extraer archivo")

def set_cache(request):
    return HttpResponse("En cache")
