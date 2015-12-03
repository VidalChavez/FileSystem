from django.conf.urls import url
from . import views


urlpatterns = [
    #url(r'^$', views.archivos, name='archivos'),
    url(r'^agrega/', views.agrega_archivo, name='agrega_archivo'),
    url(r'^extraer/', views.extraer_archivo, name='extraer_archivo'),
    url(r'^elimina/', views.elimina_archivo, name='elimina_archivo'),
]
