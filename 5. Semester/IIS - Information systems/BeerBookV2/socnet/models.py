# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Make sure each ForeignKey has `on_delete` set to the desired behavior.
#   * Remove `managed = True` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
from django.db import models
from django.contrib.auth.models import AbstractUser, BaseUserManager
from django.utils.translation import gettext_lazy as _
from django.contrib.contenttypes.fields import GenericForeignKey
from django.contrib.contenttypes.models import ContentType
from django.conf import settings

from . import utils


class BBUserManager(BaseUserManager):
    def create_user(self, email, first_name, last_name, gender, birthdate, password=None):
        if not email:
            raise ValueError('Users must have an email address')

        user = self.model(
            identifier=self.construct_identifier(first_name, last_name),
            email=self.normalize_email(email),
            first_name=first_name,
            last_name=last_name,
            gender=gender,
            birthdate=birthdate,
        )

        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_superuser(self, email, first_name, last_name, gender, birthdate, password):
        user = self.create_user(
            email,
            password=password,
            first_name=first_name,
            last_name=last_name,
            gender=gender,
            birthdate=birthdate,
        )
        user.is_admin = True
        user.is_staff = True
        user.is_superuser = True
        user.save(using=self._db)
        return user

    def construct_identifier(self, first_name, last_name):
        search_string = utils.name_to_identifier(first_name, last_name)
        users = self.filter(identifier__startswith=search_string)
        next_id = 1
        key = lambda x: int(x.identifier.split('.')[-1])
        try:
            next_id = key(max(users, key=key)) + 1
        except ValueError:
            pass
        return '.'.join([search_string, str(next_id)])


class Album(models.Model):
    id = models.OneToOneField('Content', models.CASCADE, db_column='id', primary_key=True)
    name = models.CharField(max_length=32)
    description = models.CharField(max_length=256, blank=True, null=True)
    creationtime = models.DateTimeField()
    creationplace = models.CharField(max_length=32, blank=True, null=True)
    createdby = models.ForeignKey('BBUser', models.CASCADE, db_column='createdby')
    privacy = models.CharField(max_length=9)
    titlephoto = models.ForeignKey('Photo', models.CASCADE, db_column='titlephoto', blank=True, null=True)

    class Meta:
        managed = True
        db_table = 'album'

    def __str__(self):
        return self.name


class Commentary(models.Model):
    cmnt_id = models.AutoField(primary_key=True)
    contents = models.CharField(max_length=512)
    senton = models.DateTimeField()
    place = models.CharField(max_length=32, blank=True, null=True)
    sender = models.ForeignKey('BBUser', models.CASCADE, db_column='sender')
    containedin = models.ForeignKey('Content', models.CASCADE, db_column='containedin')

    class Meta:
        managed = True
        db_table = 'commentary'

    def __str__(self):
        return self.sender.get_full_name() + ': ' + self.contents


class Content(models.Model):
    contentid = models.AutoField(primary_key=True)
    content_ref = models.ForeignKey('Event', models.CASCADE)

    class Meta:
        managed = True
        db_table = 'content'


class Conversation(models.Model):
    convid = models.AutoField(primary_key=True)
    name = models.CharField(max_length=96)
    beginning = models.DateTimeField()

    class Meta:
        managed = True
        db_table = 'conversation'

    def __str__(self):
        return self.name


class Event(models.Model):
    eventid = models.AutoField(db_column='eventid', primary_key=True)
    name = models.CharField(max_length=32)
    description = models.CharField(max_length=256, blank=True, null=True)
    event_photo = models.ImageField(upload_to='photos/events/', blank=True, null=True)
    creationtime = models.DateTimeField()
    creationplace = models.CharField(max_length=32, blank=True, null=True)
    createdby = models.ForeignKey('BBUser', models.CASCADE, db_column='createdby')
    eventlocation = models.CharField(max_length=64)
    eventtime = models.DateTimeField()
    active = models.BooleanField(default=True)

    class Meta:
        managed = True
        db_table = 'event'

    def __str__(self):
        return self.name

    def get_cname(self):
        return self.__class__.__name__

    def get_event_photo(self):
        return (settings.MEDIA_URL + self.event_photo.name) if self.event_photo else (
                settings.STATIC_URL + "default_event.png")


class Keyword(models.Model):
    keywordid = models.AutoField(primary_key=True)
    keyword = models.CharField(max_length=32)

    class Meta:
        managed = True
        db_table = 'keyword'

    def __str__(self):
        return self.keyword


class Message(models.Model):
    msg_id = models.AutoField(primary_key=True)
    subject = models.CharField(max_length=32, blank=True, null=True)
    contents = models.CharField(max_length=512)
    senton = models.DateTimeField()
    place = models.CharField(max_length=32, blank=True, null=True)
    sender = models.ForeignKey('BBUser', models.CASCADE, db_column='sender')
    partof = models.ForeignKey(Conversation, models.CASCADE, db_column='partof')

    class Meta:
        managed = True
        db_table = 'message'

    def __str__(self):
        return self.sender.get_full_name() + ': ' + self.contents


class Photo(models.Model):
    id = models.OneToOneField(Content, models.CASCADE, db_column='id', primary_key=True)
    name = models.CharField(max_length=32)
    description = models.CharField(max_length=256, blank=True, null=True)
    photo = models.ImageField(upload_to='photos/photos/')
    creationtime = models.DateTimeField()
    creationplace = models.CharField(max_length=32, blank=True, null=True)
    createdby = models.ForeignKey('BBUser', models.CASCADE, db_column='createdby')
    captureplace = models.CharField(max_length=32, blank=True, null=True)
    capturedate = models.DateField(blank=True, null=True)
    belongsto = models.ForeignKey(Album, models.CASCADE, db_column='belongsto', blank=True, null=True)

    class Meta:
        managed = True
        db_table = 'photo'

    def __str__(self):
        return self.name + ': ' + self.description

    def get_cname(self):
        return self.__class__.__name__


class Post(models.Model):
    id = models.OneToOneField(Content, models.CASCADE, db_column='id', primary_key=True)
    name = models.CharField(max_length=32)
    description = models.CharField(max_length=256, blank=True, null=True)
    creationtime = models.DateTimeField()
    creationplace = models.CharField(max_length=32, blank=True, null=True)
    createdby = models.ForeignKey('BBUser', models.CASCADE, db_column='createdby')
    contents = models.CharField(max_length=512)

    class Meta:
        managed = True
        db_table = 'post'

    def __str__(self):
        return self.contents

    def get_cname(self):
        return self.__class__.__name__


class Postkeyword(models.Model):
    pk_id = models.AutoField(primary_key=True)
    post = models.ForeignKey(Post, models.CASCADE, db_column='post')
    keyword = models.ForeignKey(Keyword, models.CASCADE, db_column='keyword')

    class Meta:
        managed = True
        db_table = 'postkeyword'
        unique_together = (('post', 'keyword'),)


class Friendship(models.Model):
    r_id = models.AutoField(primary_key=True)
    usr1 = models.ForeignKey('BBUser', models.CASCADE, db_column='usr1', related_name='FUsr1')
    usr2 = models.ForeignKey('BBUser', models.CASCADE, db_column='usr2', related_name='FUsr2')
    pending = models.BooleanField(default=True)

    class Meta:
        managed = True
        db_table = 'fiendship'
        unique_together = (('usr1', 'usr2'),)

    def __str__(self):
        return " ".join([self.usr1.get_full_name(), 'with', self.usr2.get_full_name(), self.pending.__str__()])

    def notif(self):
        return self.__name__()


class Relationship(models.Model):
    r_id = models.AutoField(primary_key=True)
    startdate = models.DateField()
    usr1 = models.ForeignKey('BBUser', models.CASCADE, db_column='usr1', related_name='Usr1')
    usr2 = models.ForeignKey('BBUser', models.CASCADE, db_column='usr2', related_name='Usr2')

    class Meta:
        managed = True
        db_table = 'relationship'
        unique_together = (('usr1', 'usr2'),)

    def __str__(self):
        return " ".join([self.usr1.get_full_name(), 'subscribed to', self.usr2.identifier.get_full_name()])


class BBUser(AbstractUser):
    identifier = models.CharField(max_length=255, unique=True)
    email = models.EmailField(_('email address'), primary_key=True)
    first_name = models.CharField(_('first name'), max_length=30, blank=False, null=False)
    last_name = models.CharField(_('last name'), max_length=150, blank=False, null=False)
    profile_pic = models.ImageField(upload_to='photos/photos/', db_column='profile_pic', null=True, blank=True)
    gender = models.CharField(max_length=11)
    birthdate = models.DateField(_('date of birth'))
    birthplace = models.CharField(max_length=32, blank=True, null=True)
    residence = models.CharField(max_length=32, blank=True, null=True)
    familystatus = models.CharField(max_length=15, blank=True, null=True)
    education = models.CharField(max_length=32, blank=True, null=True)
    workplace = models.CharField(max_length=32, blank=True, null=True)

    username = None

    objects = BBUserManager()

    USERNAME_FIELD = 'email'
    REQUIRED_FIELDS = ['first_name', 'last_name', 'gender', 'birthdate']

    class Meta:
        managed = True
        db_table = 'usr'

    def __str__(self):
        return " ".join([self.first_name, self.last_name, str(self.email)])

    def get_username(self):
        return self.email

    def get_profile_pic(self):
        return (settings.MEDIA_URL + self.profile_pic.name) if self.profile_pic else (
                settings.STATIC_URL + "default_profile_pic.png")


class Usrconversation(models.Model):
    uc_id = models.AutoField(primary_key=True)
    usr = models.ForeignKey(BBUser, models.CASCADE, db_column='usr')
    conversation = models.ForeignKey(Conversation, models.CASCADE, db_column='conversation')

    class Meta:
        managed = True
        db_table = 'usrconversation'
        unique_together = (('usr', 'conversation'),)

    def __str__(self):
        return self.usr.get_full_name() + ' in conversation ' + str(self.conversation)


class Usrevent(models.Model):
    ue_id = models.AutoField(primary_key=True)
    usr = models.ForeignKey(BBUser, models.CASCADE, db_column='usr')
    event = models.ForeignKey(Event, models.CASCADE, db_column='event')

    class Meta:
        managed = True
        db_table = 'usrevent'
        unique_together = (('usr', 'event'),)

    def __str__(self):
        return self.usr.get_full_name() + ' going to event ' + str(self.event)


class Usrphoto(models.Model):
    up_id = models.AutoField(primary_key=True)
    usr = models.ForeignKey(BBUser, models.CASCADE, db_column='usr')
    photo = models.ForeignKey(Photo, models.CASCADE, db_column='photo')

    class Meta:
        managed = True
        db_table = 'usrphoto'
        unique_together = (('usr', 'photo'),)

    def __str__(self):
        return self.usr.get_full_name() + ' tagged at photo ' + str(self.photo)


class Usrpost(models.Model):
    up_id = models.AutoField(primary_key=True)
    usr = models.ForeignKey(BBUser, models.CASCADE, db_column='usr')
    post = models.ForeignKey(Post, models.CASCADE, db_column='post')

    class Meta:
        managed = True
        db_table = 'usrpost'
        unique_together = (('usr', 'post'),)

    def __str__(self):
        return self.usr.get_full_name() + ' tagged in post ' + str(self.post)


class Notification(models.Model):
    id = models.AutoField(primary_key=True)
    usr = models.ForeignKey(BBUser, models.CASCADE, db_column='usr')
    content_type = models.ForeignKey(ContentType, on_delete=models.CASCADE)
    creationtime = models.DateTimeField()
    title = models.CharField(max_length=32)
    content = models.CharField(max_length=256, blank=True, null=True)
    object_id = models.PositiveIntegerField()
    content_object = GenericForeignKey()

    class Meta:
        managed = True
        db_table = 'notification'

    def __str__(self):
        return self.usr.get_full_name() + ': ' + self.title
