from django.shortcuts import render, redirect
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt


def archivos(request):
    return render(request, "files/dashboard.html", {'archivos':True})

def agrega_archivo_path(request):
    if request.method == 'POST':
        path = request.POST.get('path')
        msg = "El path para el nuevo archivo es: " + path
        print msg
        print "*******************"
        return redirect('/')
    else:
        return render(request, 'files/input_path_file.html')

def elimina_archivo(request, nombre_archivo):
    msg = "Elimina Archivo " + nombre_archivo
    print msg
    print "xxxxxxxxxxxxxxx"
    return redirect('/')

def extraer_path(request, nombre_archivo):
    if request.method == 'POST':
        path = request.POST.get('path')
        msg = "Extraer Path de" + nombre_archivo + path
        print msg
        print "*******************"
        return redirect('/')
    else:
        return render(request, 'files/input_path.html')

def extraer_archivo(request):
    return HttpResponse("Extraer archivo")

@csrf_exempt
def set_cache(request):
    if request.is_ajax():
        if request.method == 'GET':
            #
            return JsonResponse({'status': 'ok'})
    else:
        return Http404("Error")

def graficas(request):
    return render(request, 'files/graphs.html', {'graficas':True})
