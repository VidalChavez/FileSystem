from django.conf.urls import url
from . import views


urlpatterns = [
    # url(r'^$', views.archivos, name='archivos'),
    url(r'^agrega_archivo_path/', views.agrega_archivo_path, name='agrega_archivo_path'),
    # url(r'^extraer/', views.extraer_archivo, name='extraer_archivo'),
    url(r'^elimina/(?P<nombre_archivo>[a-zA-Z0-9_ ]+)/$', views.elimina_archivo, name='elimina_archivo'),
    url(r'^extraer_path/(?P<nombre_archivo>[a-zA-Z0-9_ ]+)/$', views.extraer_path, name='extraer_path'),
    url(r'^set_cache/', views.set_cache, name='set_cache'),
    url(r'^set_dd/', views.set_dd, name='set_dd'),
    url(r'^graficas/', views.graficas, name='graficas'),
]
