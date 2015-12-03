from django.conf.urls import url
from . import views


urlpatterns = [
    # url(r'^$', views.archivos, name='archivos'),
    url(r'^agrega/', views.agrega_archivo, name='agrega_archivo'),
    # url(r'^extraer/', views.extraer_archivo, name='extraer_archivo'),
    url(r'^elimina/(?P<nombre_archivo>[a-zA-Z0-9_ ]+)/$', views.elimina_archivo, name='elimina_archivo'),
    url(r'^extraer_path/(?P<nombre_archivo>[a-zA-Z0-9_ ]+)/$', views.extraer_path, name='extraer_path'),
]
