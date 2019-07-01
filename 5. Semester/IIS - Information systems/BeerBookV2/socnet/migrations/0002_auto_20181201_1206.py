# Generated by Django 2.1.3 on 2018-12-01 12:06

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('socnet', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='Friendship',
            fields=[
                ('r_id', models.AutoField(primary_key=True, serialize=False)),
                ('usr1', models.ForeignKey(db_column='usr1', on_delete=django.db.models.deletion.DO_NOTHING, related_name='FUsr1', to=settings.AUTH_USER_MODEL)),
                ('usr2', models.ForeignKey(db_column='usr2', on_delete=django.db.models.deletion.DO_NOTHING, related_name='FUsr2', to=settings.AUTH_USER_MODEL)),
            ],
            options={
                'db_table': 'fiendship',
                'managed': True,
            },
        ),
        migrations.AlterUniqueTogether(
            name='friendship',
            unique_together={('usr1', 'usr2')},
        ),
    ]
