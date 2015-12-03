from django.conf.urls import url, include
from django.contrib import admin

urlpatterns = [
    url(r'^$', 'files.views.archivos'),
    url(r'^files/', include('files.urls', namespace='files', app_name='files')),
]
