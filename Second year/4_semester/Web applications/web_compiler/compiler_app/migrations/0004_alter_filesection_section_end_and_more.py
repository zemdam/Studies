# Generated by Django 4.2.1 on 2023-05-24 19:23

from django.db import migrations, models


class Migration(migrations.Migration):
    dependencies = [
        ("compiler_app", "0003_alter_file_actual_file"),
    ]

    operations = [
        migrations.AlterField(
            model_name="filesection",
            name="section_end",
            field=models.PositiveIntegerField(),
        ),
        migrations.AlterField(
            model_name="filesection",
            name="section_start",
            field=models.PositiveIntegerField(),
        ),
    ]