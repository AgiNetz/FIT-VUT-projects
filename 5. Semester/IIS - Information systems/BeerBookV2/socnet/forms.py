# users/forms.py
from django import forms
from django.contrib.auth.forms import UserCreationForm, UserChangeForm, AuthenticationForm
from django.forms.widgets import PasswordInput, TextInput
from .models import BBUser, Event
from django.contrib.auth.forms import UserCreationForm, UserChangeForm
from .models import BBUser, Event, Photo, Message, Post
import datetime

familystatus_choices = [('single', 'single'), ('married', 'married'),
                        ('divorced', 'divorced'), ('widowed', 'widowed'),
                        ('in relationship', 'in relationship')]


class BBUserCreationForm(UserCreationForm):

    class Meta(UserCreationForm):
        model = BBUser
        fields = ('email', 'first_name', 'last_name', 'password1', 'password2', 'gender', 'birthdate')
        widgets = {'birthdate': forms.SelectDateWidget(years=list(range(1900, datetime.date.today().year))),
                   'gender': forms.Select(choices=[('Male', 'Male'), ('Female', 'Female'), ('Unspecified', 'Unspecified')])}


class BBUserChangeForm(UserChangeForm):

    class Meta(UserChangeForm):
        model = BBUser
        fields = '__all__'
        field_classes = {}
        pass


class BBUserForm(forms.ModelForm):

    class Meta:
        model = BBUser
        fields = ('profile_pic', 'first_name', 'last_name', 'gender', 'birthdate', 'birthplace',
                  'residence', 'familystatus', 'education', 'workplace',
                  'email',)
        field_classes = {'email': forms.EmailField,
                         }
        widgets = {'gender': forms.Select(choices=[('Male', 'Male'), ('Female', 'Female'), ('Unspecified', 'Unspecified')]),
                   'familystatus': forms.Select(choices=familystatus_choices),
                   'birthdate': forms.SelectDateWidget(years=list(range(1900, datetime.date.today().year))),
                   }
        labels = {'profile_pic': 'Profile picture'}


class BBUserViewForm(forms.ModelForm):

    class Meta:
        model = BBUser
        fields = ('first_name', 'last_name', 'gender', 'birthdate', 'birthplace',
                  'residence', 'familystatus', 'education', 'workplace',
                  'email',)
        labels = {'profile_pic': 'Profile picture'}


class EventForm(forms.ModelForm):

    class Meta:
        model = Event
        fields = ('name', 'description', 'event_photo', 'eventlocation', 'eventtime',)
        widgets = {'description': forms.Textarea(),
                   'eventtime': forms.SelectDateWidget()}
        labels = {'eventlocation': 'Location',
                  'eventtime': 'Time',}


class EventViewForm(forms.ModelForm):

    class Meta:
        model = Event
        fields = ('name', 'description',  'eventlocation', 'eventtime', 'createdby',)
        labels = {'eventlocation': 'Location',
                  'eventtime': 'Time',
                  'createdby': 'Created by'}


class LetsHangLoginForm(AuthenticationForm):
    """Custom login form redefining username and password text input fields."""
    username = forms.CharField(widget=TextInput(attrs={'class': 'validate form-control', 'placeholder': 'E-mail'}))
    password = forms.CharField(widget=PasswordInput(attrs={'class': 'form-control', 'placeholder':'Password'}))


class PhotoForm(forms.ModelForm):

    atevent = forms.BooleanField()

    class Meta:
        model = Photo
        fields = ('name', 'description', 'photo', 'atevent')
        labels = {'atevent': 'At event'}

    def __init__(self, event_choices, *args, **kwargs):
        super(forms.ModelForm, self).__init__(*args, **kwargs)
        self.fields['atevent'] = forms.ChoiceField(choices=event_choices, required=True)


class NewMessageForm(forms.ModelForm):

    receiver = forms.ModelMultipleChoiceField(None)

    class Meta:
        model = Message
        fields = ('contents', )
        labels = {'contents': 'Text'}
        widgets = {'contents': forms.Textarea()}
        
    def __init__(self, user_choices, *args, **kwargs):
        super(forms.ModelForm, self).__init__(*args, **kwargs)
        self.fields['receiver'] = forms.MultipleChoiceField(choices=user_choices, required=True)
        
        
class ConversationForm(forms.ModelForm):
    
    class Meta:
        model = Message
        fields = ('contents', )
        labels = {'contents': 'Text'}
        widgets = {'contents': forms.Textarea()}


class PostForm(forms.ModelForm):

    atevent = forms.BooleanField()

    class Meta:
        model = Post
        fields = ('contents',)
        labels = {'contents': 'Text',
                  'atevent': 'At event',}
        widgets = {'contents': forms.Textarea()}

    def __init__(self, event_choices, *args, **kwargs):
        super(forms.ModelForm, self).__init__(*args, **kwargs)
        self.fields['atevent'] = forms.ChoiceField(choices=event_choices, required=True)


class PostToEventForm(forms.ModelForm):

    class Meta:
        model = Post
        fields = ('contents',)
        labels = {'contents': 'Text',}
        widgets = {'contents': forms.Textarea()}
