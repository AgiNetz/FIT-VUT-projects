from django.contrib import admin
from django.contrib.auth import get_user_model
from django.contrib.auth.admin import UserAdmin

from .forms import BBUserCreationForm, BBUserChangeForm
from .models import *


class BBUserAdmin(UserAdmin):
    add_form = BBUserCreationForm
    form = BBUserChangeForm
    model = BBUser
    list_display = ['email', 'first_name', 'last_name']
    search_fields = ('first_name', 'last_name', 'email')
    ordering = ('email',)
    fieldsets = None
    add_fieldsets = None


admin.site.register(BBUser, BBUserAdmin)
admin.site.register(Relationship)
admin.site.register(Event)
admin.site.register(Content)
admin.site.register(Notification)
admin.site.register(Post)
admin.site.register(Photo)
admin.site.register(Conversation)
admin.site.register(Usrconversation)
admin.site.register(Message)
admin.site.register(Commentary)
admin.site.register(Friendship)
admin.site.register(Usrevent)
admin.site.register(Usrphoto)
admin.site.register(Usrpost)
