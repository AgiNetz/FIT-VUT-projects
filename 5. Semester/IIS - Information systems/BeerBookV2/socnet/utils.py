import unicodedata
from django.shortcuts import get_object_or_404
from . import models
from itertools import chain
from operator import attrgetter


def get_event(eventid):
    return get_object_or_404(models.Event, eventid=eventid, active=True)


def get_event_list(**kwargs):
    return models.Event.objects.filter(active=True, **kwargs)


def get_user(identifier):
    return get_object_or_404(models.BBUser, identifier__exact=identifier, is_active=True)


def get_user_list(**kwargs):
    return models.BBUser.objects.filter(is_active=True, **kwargs)


def normalize_string(string):
    return ''.join(unicodedata.normalize('NFKD', string).encode('ASCII', 'ignore').decode('ASCII').lower().split())


def name_to_identifier(first_name, last_name):
    first_norm = normalize_string(first_name)
    last_norm = normalize_string(last_name)
    return '.'.join([first_norm, last_norm])


def normalize_name(search_string):
    """
    Function for input user combinations. Used in later querries to database.
    Transforms user input into standardized format 'first_name', 'last_name'.
    Those values can be withdrawn from returned list of dictionaries. Where
    first key represents 'first_name' and second 'last_name' accordignly.

    :param search_string: Input to be provided to helper function.
    :return:List of dictionaries filled with combinations of fields into 'first_name'
            and 'last_name' accordingly.
    """
    splitted = search_string.split(' ')
    normalized = []

    for i in range(search_string.count(' ') + 2):
        first_name = "".join([s for s in splitted[:i]])
        last_name = "".join([s for s in splitted[i:]])
        normalized.append({'first_name': first_name, 'last_name': last_name})

    return normalized


def get_feed(usr_id=None, evnt_id=None):
    if usr_id is None and evnt_id is None:
        return sorted(chain(models.Event.objects.all(), models.Album.objects.all(),
                            models.Photo.objects.all(), models.Post.objects.all()),
                            key=attrgetter('creationtime'), reverse=True)
    if evnt_id is None:
        return sorted(chain(models.Album.objects.filter(createdby__identifier=usr_id), 
                models.Event.objects.filter(createdby__identifier=usr_id),
                models.Photo.objects.filter(createdby__identifier=usr_id),
                models.Post.objects.filter(createdby__identifier=usr_id)),
                key=attrgetter('creationtime'), reverse=True)
    if usr_id is None:
        return sorted(chain(models.Album.objects.filter(id__content_ref__eventid=evnt_id),
                        models.Photo.objects.filter(id__content_ref__eventid=evnt_id), 
                        models.Post.objects.filter(id__content_ref__eventid=evnt_id)),
                        key=attrgetter('creationtime'), reverse=True)

def get_comments(content_id):
    return Commentary.objects.find(containedin__contentid=content_id)
    