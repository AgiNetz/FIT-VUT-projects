# Generated by Django 2.1.3 on 2018-12-02 12:34

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('socnet', '0008_auto_20181202_1114'),
    ]

    operations = [
        migrations.AlterField(
            model_name='content',
            name='contentid',
            field=models.IntegerField(primary_key=True, serialize=False),
        ),
        migrations.AlterField(
            model_name='event',
            name='eventid',
            field=models.AutoField(db_column='eventid', primary_key=True, serialize=False),
        ),
    ]
