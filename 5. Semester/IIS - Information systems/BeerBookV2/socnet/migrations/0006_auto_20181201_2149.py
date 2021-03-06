# Generated by Django 2.1.3 on 2018-12-01 21:49

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('socnet', '0005_auto_20181201_2044'),
    ]

    operations = [
        migrations.AlterField(
            model_name='album',
            name='albumid',
            field=models.OneToOneField(db_column='albumid', on_delete=django.db.models.deletion.CASCADE, primary_key=True, serialize=False, to='socnet.Content'),
        ),
        migrations.AlterField(
            model_name='album',
            name='createdby',
            field=models.ForeignKey(db_column='createdby', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='album',
            name='titlephoto',
            field=models.ForeignKey(blank=True, db_column='titlephoto', null=True, on_delete=django.db.models.deletion.CASCADE, to='socnet.Photo'),
        ),
        migrations.AlterField(
            model_name='bbuser',
            name='profile_pic',
            field=models.ForeignKey(blank=True, db_column='profile_pic', null=True, on_delete=django.db.models.deletion.CASCADE, to='socnet.Photo'),
        ),
        migrations.AlterField(
            model_name='commentary',
            name='containedin',
            field=models.ForeignKey(db_column='containedin', on_delete=django.db.models.deletion.CASCADE, to='socnet.Content'),
        ),
        migrations.AlterField(
            model_name='commentary',
            name='sender',
            field=models.ForeignKey(db_column='sender', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='event',
            name='createdby',
            field=models.ForeignKey(db_column='createdby', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='event',
            name='eventid',
            field=models.OneToOneField(db_column='eventid', on_delete=django.db.models.deletion.CASCADE, primary_key=True, serialize=False, to='socnet.Content'),
        ),
        migrations.AlterField(
            model_name='friendship',
            name='usr1',
            field=models.ForeignKey(db_column='usr1', on_delete=django.db.models.deletion.CASCADE, related_name='FUsr1', to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='friendship',
            name='usr2',
            field=models.ForeignKey(db_column='usr2', on_delete=django.db.models.deletion.CASCADE, related_name='FUsr2', to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='message',
            name='partof',
            field=models.ForeignKey(db_column='partof', on_delete=django.db.models.deletion.CASCADE, to='socnet.Conversation'),
        ),
        migrations.AlterField(
            model_name='message',
            name='sender',
            field=models.ForeignKey(db_column='sender', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='notification',
            name='usr',
            field=models.ForeignKey(db_column='usr', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='photo',
            name='belongsto',
            field=models.ForeignKey(blank=True, db_column='belongsto', null=True, on_delete=django.db.models.deletion.CASCADE, to='socnet.Album'),
        ),
        migrations.AlterField(
            model_name='photo',
            name='createdby',
            field=models.ForeignKey(db_column='createdby', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='photo',
            name='photoid',
            field=models.OneToOneField(db_column='photoid', on_delete=django.db.models.deletion.CASCADE, primary_key=True, serialize=False, to='socnet.Content'),
        ),
        migrations.AlterField(
            model_name='post',
            name='createdby',
            field=models.ForeignKey(db_column='createdby', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='post',
            name='postid',
            field=models.OneToOneField(db_column='postid', on_delete=django.db.models.deletion.CASCADE, primary_key=True, serialize=False, to='socnet.Content'),
        ),
        migrations.AlterField(
            model_name='postkeyword',
            name='keyword',
            field=models.ForeignKey(db_column='keyword', on_delete=django.db.models.deletion.CASCADE, to='socnet.Keyword'),
        ),
        migrations.AlterField(
            model_name='postkeyword',
            name='post',
            field=models.ForeignKey(db_column='post', on_delete=django.db.models.deletion.CASCADE, to='socnet.Post'),
        ),
        migrations.AlterField(
            model_name='relationship',
            name='usr1',
            field=models.ForeignKey(db_column='usr1', on_delete=django.db.models.deletion.CASCADE, related_name='Usr1', to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='relationship',
            name='usr2',
            field=models.ForeignKey(db_column='usr2', on_delete=django.db.models.deletion.CASCADE, related_name='Usr2', to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='usrconversation',
            name='conversation',
            field=models.ForeignKey(db_column='conversation', on_delete=django.db.models.deletion.CASCADE, to='socnet.Conversation'),
        ),
        migrations.AlterField(
            model_name='usrconversation',
            name='usr',
            field=models.ForeignKey(db_column='usr', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='usrevent',
            name='event',
            field=models.ForeignKey(db_column='event', on_delete=django.db.models.deletion.CASCADE, to='socnet.Event'),
        ),
        migrations.AlterField(
            model_name='usrevent',
            name='usr',
            field=models.ForeignKey(db_column='usr', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='usrphoto',
            name='photo',
            field=models.ForeignKey(db_column='photo', on_delete=django.db.models.deletion.CASCADE, to='socnet.Photo'),
        ),
        migrations.AlterField(
            model_name='usrphoto',
            name='usr',
            field=models.ForeignKey(db_column='usr', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='usrpost',
            name='post',
            field=models.ForeignKey(db_column='post', on_delete=django.db.models.deletion.CASCADE, to='socnet.Post'),
        ),
        migrations.AlterField(
            model_name='usrpost',
            name='usr',
            field=models.ForeignKey(db_column='usr', on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
    ]
