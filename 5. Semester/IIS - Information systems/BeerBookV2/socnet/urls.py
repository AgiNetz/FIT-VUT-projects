from django.urls import path
from django.conf import settings
from django.conf.urls import url
from django.conf.urls.static import static
from . import views
from django.contrib.auth import views as auth_views
from .forms import LetsHangLoginForm

app_name = 'socnet'
urlpatterns = [
    path('', views.index, name='index'),
    path('signup/', views.signup, name='signup'),
    path('<identifier>', views.view_profile, name='view_profile'),
    path('<identifier>/edit/', views.edit_user, name='edit_user'),
    path('<identifier>/free/', views.delete_user, name='delete_user'),
    path('<identifier>/sub/', views.subscribe, name='subscribe'),
    path('<identifier>/unsub/', views.unsubscribe, name='unsubscribe'),
    path('<identifier>/friend/', views.friend, name='friend'),
    path('<identifier>/unfriend/', views.unfriend, name='unfriend'),
    path('<identifier>/accept/', views.accept_friendship, name='accept_friendship'),
    path('<identifier>/cancel/', views.cancel_friendship, name='cancel_friendship'),
    path('<identifier>/photos', views.user_photos, name='user_photos'),
    path('<identifier>/profile', views.user_profile, name='user_profile'),
    path('<identifier>/events', views.user_events, name='user_events'),
    path('friends/', views.view_friends, name='view_friends'),
    path('events/', views.events_index, name='events_index'),
    path('events/new_event', views.new_event, name='new_event'),
    path('events/your_events', views.your_events, name='your_events'),
    path('events/<eventid>', views.event_detail, name='event_detail'),
    path('events/<eventid>/edit', views.event_edit, name='event_edit'),
    path('events/<eventid>/delete', views.event_delete, name='event_delete'),
    path('events/<eventid>/going', views.event_going, name='event_going'),
    path('events/<eventid>/not_going', views.event_not_going, name='event_not_going'),
    path('events/<eventid>/post', views.publish_post_to_event, name='publish_post_to_event'),
    path('notifications/', views.notifications, name='notifications'),
    url(r'^login/$', auth_views.LoginView.as_view(authentication_form=LetsHangLoginForm,
                                                  template_name='registration/login.html')),
    path('upload_photo/', views.upload_photo, name='upload_photo'),
    path('new_message/', views.send_message, name='send_message'),
    path('conversations/', views.conversation_view, name='conversation_view'),
    path('conversations/<convid>', views.conversation_detail, name='conversation_detail'),
    path('publish_post/', views.publish_post, name='publish_post'),
    path('post/', views.publish_post_from_profile, name='publish_post_from_profile'),
    path('search/', views.search_result, name='search_result')
] + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT) \
  + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)

