from django.shortcuts import render, redirect, get_object_or_404
from django.http import HttpResponse, HttpResponseRedirect, Http404
from django.urls import reverse
from django.conf import settings
from django.contrib import messages
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django.utils import timezone
from datetime import date
from django.contrib.contenttypes.models import ContentType

from .forms import *
from .models import *
from .utils import *


@login_required(login_url='login')
def index(request):
    users_list = []
    names = []
    comments = []
    random = []
    
    feed_list = get_feed()
    for element in feed_list:
        if not isinstance(element, Event):
            comments.append(Commentary.objects.filter(containedin=element.id))
        else:
            comments.append(None)
    
    result_list = zip(feed_list, comments)
    for item in result_list:
        random.append(item)

    event_keys = Usrevent.objects.filter(usr=request.user).values_list('event', flat=True)
    user_events = Event.objects.filter(pk__in=event_keys).values_list('eventid', 'name')
    post_content_form = PostForm(user_events)
    
    if request.method == 'POST' and request.POST['form_identifier']:
        Commentary.objects.create(contents=request.POST['comment_field'], senton=date.today(),
                                  sender=request.user, containedin=Content.objects.get(contentid=request.POST['form_identifier']))

    if request.method == 'POST' and request.POST.get('search_box') is not None:
        if request.POST.get('search_box') != '' and len(request.POST.get('search_box').split(' ')) == 1:
            for result in get_user_list(identifier__startswith=normalize_string(request.POST.get('search_box'))):
                users_list.append(result)
        if request.POST.get('search_box') != '':
            search_strings = normalize_name(request.POST.get('search_box'))
            for string in search_strings:
                names.append(name_to_identifier(string['first_name'], string['last_name']))
            for name in names:
                for result in get_user_list(identifier__contains=name):
                    users_list.append(result)
        else:
            users_list = get_user_list()
    else:
        users_list = get_user_list()
        
    return render(request, 'socnet/index.html', {'users_list': set(users_list), 'contents': random, 'post_content_form': post_content_form})


def signup(request):
    if request.POST:
        form = BBUserCreationForm(request.POST)
        if form.is_valid():
            new_form = form.save(commit=False)
            new_form.identifier = BBUser.objects.construct_identifier(request.POST['first_name'],
                                                                      request.POST['last_name'])
            new_form.save()
            messages.success(request, 'Registration successful! You can now log in.')
            return HttpResponseRedirect(reverse('socnet:index'))
        else:
            context = {'form': form}
            return render(request, 'socnet/signup.html', context)
    else:

        context = {'form': BBUserCreationForm}
        return render(request, 'socnet/signup.html', context)


@login_required(login_url='login')
def view_profile(request, identifier):
    user = get_user(identifier)
    form = BBUserViewForm(instance=user)
    profile_pic = user.get_profile_pic()
    comments = []
    random = []

    usr1 = request.user
    usr2 = user

    feed_list = get_feed(usr_id=identifier)
    for element in feed_list:
        if not isinstance(element, Event):
            comments.append(Commentary.objects.filter(containedin=element.id))
        else:
            comments.append(None)
    
    result_list = zip(feed_list, comments)
    for item in result_list:
        random.append(item)

    event_keys = Usrevent.objects.filter(usr=request.user).values_list('event', flat=True)
    user_events = Event.objects.filter(pk__in=event_keys).values_list('eventid', 'name')
    post_content_form = PostForm(user_events)

    context = {'form': form, 'profile_pic': profile_pic, 'contents': random, 'post_content_form': post_content_form}
    if request.user != user:
        if Relationship.objects.filter(usr1__exact=usr1) & Relationship.objects.filter(usr2__exact=usr2):
            relationship = 'subscribed'
        else:
            relationship = 'notsubscribed'
        friendship = None
        if (Friendship.objects.filter(usr1__exact=usr1).filter(usr2__exact=usr2) or
                Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=False)):
            friendship = 'friends'
        elif Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=True):
            friendship = 'requested'
        context = {**context, 'relationship': relationship, 'friendship': friendship}
    return render(request, 'socnet/view_user_feed.html', context)


@login_required(login_url='login')
def user_profile(request, identifier):
    user = get_user(identifier)
    form = BBUserViewForm(instance=user)
    profile_pic = user.get_profile_pic()

    usr1 = request.user
    usr2 = user

    context = {'form': form, 'profile_pic': profile_pic}
    if request.user != user:
        if Relationship.objects.filter(usr1__exact=usr1) & Relationship.objects.filter(usr2__exact=usr2):
            relationship = 'subscribed'
        else:
            relationship = 'notsubscribed'
        friendship = None
        if (Friendship.objects.filter(usr1__exact=usr1).filter(usr2__exact=usr2) or
                Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=False)):
            friendship = 'friends'
        elif Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=True):
            friendship = 'requested'
        context = {**context, 'relationship': relationship, 'friendship': friendship}

    return render(request, 'socnet/view_user_profile.html', context)


@login_required(login_url='login')
def edit_user(request, identifier):
    user = get_user(identifier)
    if request.user != user:
        return redirect('socnet:view_profile', identifier=user.identifier)

    form = BBUserForm(instance=user)
    if request.method == "POST":
        form = BBUserForm(request.POST, request.FILES, instance=user)
        if form.is_valid():
            messages.success(request, message="Your changes were saved!")
            form.save()
            return redirect('socnet:view_profile', identifier=identifier)

    return render(request, 'socnet/edit_user.html', {'form': form})


@login_required(login_url='login')
def delete_user(request, identifier):
    user = request.user
    if request.user.identifier == identifier:
        user.is_active = False
        user.save()
        logout(request)
        return render(request, 'socnet/deleted_profile.html')
    return render(request, 'socnet/deleted_profile.html', {'error': 1})


@login_required(login_url='login')
def subscribe(request, identifier):
    usr1 = request.user
    usr2 = get_object_or_404(BBUser, identifier__exact=identifier)

    Relationship.objects.get_or_create(startdate=date.today(), usr1=usr1, usr2=usr2)

    return redirect('socnet:view_profile', identifier=identifier)


@login_required(login_url='login')
def unsubscribe(request, identifier):
    usr1 = request.user
    usr2 = get_object_or_404(BBUser, identifier__exact=identifier)

    relationship = Relationship.objects.filter(usr1__exact=usr1)
    relationship.filter(usr2__exact=usr2)
    relationship.delete()

    return redirect('socnet:view_profile', identifier=identifier)


@login_required(login_url='login')
def friend(request, identifier):
    usr1 = request.user
    usr2 = get_object_or_404(BBUser, identifier__exact=identifier)

    friendship,bs = Friendship.objects.get_or_create(usr1=usr1, usr2=usr2)
    Notification.objects.get_or_create(usr=usr2, content_type=ContentType.objects.get_for_model(friendship),
                                       creationtime=date.today(),
                                       title='Friend request', content=None, object_id=friendship.r_id)


    return redirect('socnet:view_profile', identifier=identifier)


@login_required(login_url='login')
def unfriend(request, identifier):
    usr1 = request.user
    usr2 = get_object_or_404(BBUser, identifier__exact=identifier)

    Friendship.objects.filter(usr1__exact=usr1).filter(usr2__exact=usr2).delete()
    Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).delete()

    return redirect('socnet:view_profile', identifier=identifier)


@login_required(login_url='login')
def accept_friendship(request, identifier):
    usr1 = request.user
    usr2 = get_object_or_404(BBUser, identifier__exact=identifier)

    Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).update(pending=False)

    return redirect('socnet:view_profile', identifier=identifier)


@login_required(login_url='login')
def cancel_friendship(request, identifier):
    usr1 = request.user
    usr2 = get_object_or_404(BBUser, identifier__exact=identifier)

    Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).delete()

    return redirect('socnet:view_profile', identifier=identifier)


@login_required(login_url='login')
def view_friends(request):
    friendship_list = []
    friendship_list_pending = []
    names = []

    for friendship in Friendship.objects.filter(usr1=request.user).filter(pending=False):
        friendship_list.append(friendship.usr2)
    for friendship in Friendship.objects.filter(usr2=request.user).filter(pending=False):
        friendship_list.append(friendship.usr1)

    for friendship in Friendship.objects.filter(usr1=request.user).filter(pending=True):
        friendship_list_pending.append(friendship.usr2)
    for friendship in Friendship.objects.filter(usr2=request.user).filter(pending=True):
        friendship_list_pending.append(friendship.usr1)

    users_list = []
    users_list_pending = []

    if request.method == 'POST' and request.POST.get('search_box') is not None:
        if request.POST.get('search_box') != '' and len(request.POST.get('search_box').split(' ')) == 1:
            for user in friendship_list:
                if normalize_string(request.POST.get('search_box')) in user.identifier:
                    users_list.append(user)
            for user in friendship_list_pending:
                if normalize_string(request.POST.get('search_box')) in user.identifier:
                    users_list_pending.append(user)
        if request.POST.get('search_box') != '':
            search_strings = normalize_name(request.POST.get('search_box'))
            for string in search_strings:
                names.append(name_to_identifier(string['first_name'], string['last_name']))
            for name in names:
                for user in friendship_list:
                    if name in user.identifier:
                        users_list.append(user)
                for user in friendship_list_pending:
                    if name in user.identifier:
                        users_list_pending.append(user)
        else:
            users_list = friendship_list
            users_list_pending = friendship_list_pending
    else:
        users_list = friendship_list
        users_list_pending = friendship_list_pending
    return render(request, 'socnet/friendships.html', {'friends_list': set(users_list),
                                                       'friends_list_pending': set(users_list_pending)})


@login_required(login_url='login')
def events_index(request):
    event_list = get_event_list()
    return render(request, 'events/events_index.html', {'event_list': event_list})


@login_required(login_url='login')
def new_event(request):
    form = EventForm()
    if request.method == "POST":
        form = EventForm(request.POST, request.FILES)
        if form.is_valid():
            new_form = form.save(commit=False)
            new_form.createdby = request.user
            new_form.creationtime = timezone.now()
            form.save()
            messages.success(request, message='Event {0} successfully created!'.format(new_form.name))
            return redirect('socnet:events_index')
    return render(request, 'events/new_event.html', {'form': form})


@login_required(login_url='login')
def event_detail(request, eventid=None):
    comments = []
    random = []
    event = get_event(eventid)
    form = EventViewForm(instance=event)
    feed_list = get_feed(evnt_id=eventid)
    for element in feed_list:
        if not isinstance(element, Event):
            comments.append(Commentary.objects.filter(containedin=element.id))
        else:
            comments.append(None)
    
    result_list = zip(feed_list, comments)
    for item in result_list:
        random.append(item)
    post_content_form = PostToEventForm()
    going = False
    try:
        Usrevent.objects.get(usr=request.user, event=event)
        going = True
    except Usrevent.DoesNotExist:
        pass
    photo = event.get_event_photo()
    return render(request, 'events/event_detail.html', {'form': form, 'photo': photo,
                                                        'going': going, 'contents': random,
                                                        'post_content_form': post_content_form})


@login_required(login_url='login')
def your_events(request):
    past_events = get_event_list(createdby=request.user, eventtime__lt=date.today())
    current_events = get_event_list(createdby=request.user, eventtime__gte=date.today())
    return render(request, 'events/your_events.html', {'past_events': past_events, 'current_events': current_events})


@login_required(login_url='login')
def event_edit(request, eventid):
    event = get_event(eventid)
    if request.user != event.createdby:
        return redirect('socnet:event_detail', eventid=eventid)
    elif event.eventtime < timezone.now():
        messages.error(request, message="An event in the past cannot be edited!")
        return redirect('socnet:event_detail', eventid=eventid)

    form = EventForm(instance=event)
    if request.method == "POST":
        form = EventForm(request.POST, request.FILES, instance=event)
        if form.is_valid():
            messages.success(request, message="Your changes were saved!")
            form.save()
            return redirect('socnet:event_detail', eventid=eventid)

    return render(request, 'events/event_edit.html', {'form': form})


@login_required(login_url='login')
def event_going(request, eventid):
    event = get_event(eventid)
    Usrevent.objects.get_or_create(usr=request.user, event=event)
    return redirect('socnet:event_detail', eventid=eventid)


@login_required(login_url='login')
def event_not_going(request, eventid):
    event = get_event(eventid)
    try:
        ue = Usrevent.objects.get(usr=request.user, event=event)
        ue.delete()
    except Usrevent.DoesNotExist:
        pass
    return redirect('socnet:event_detail', eventid=eventid)


@login_required(login_url='login')
def event_delete(request, eventid):
    event = get_event(eventid)
    if request.user != event.createdby:
        return redirect('socnet:event_detail', eventid=eventid)
    event.active = False
    event.save()
    messages.success(request, "Event '{0}' has been successfully deleted.".format(event.name))
    return redirect('socnet:events_index')


@login_required(login_url='login')
def notifications(request):
    object_list = []
    notification_list = Notification.objects.filter(usr=request.user).order_by('-id')[:10]

    for notification in notification_list:
        if notification.content_object is None:
            notification.delete()
            continue
        object_list.append(notification)

    print(object_list)

    return render(request, 'socnet/notifications.html', {'notification_list': object_list})

@login_required(login_url='login')
def user_photos(request, identifier):
    page_owner = get_user(identifier)
    photos = Photo.objects.filter(createdby=page_owner)

    user = get_user(identifier)
    form = BBUserViewForm(instance=user)
    profile_pic = user.get_profile_pic()

    usr1 = request.user
    usr2 = user

    context = {'form': form, 'profile_pic': profile_pic, 'photos': photos}
    if request.user != user:
        if Relationship.objects.filter(usr1__exact=usr1) & Relationship.objects.filter(usr2__exact=usr2):
            relationship = 'subscribed'
        else:
            relationship = 'notsubscribed'
        friendship = None
        if (Friendship.objects.filter(usr1__exact=usr1).filter(usr2__exact=usr2) or
                Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=False)):
            friendship = 'friends'
        elif Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=True):
            friendship = 'requested'
        context = {**context, 'relationship': relationship, 'friendship': friendship}

    return render(request, 'socnet/view_user_photos.html', context)


@login_required(login_url='login')
def upload_photo(request):
    event_keys = Usrevent.objects.filter(usr=request.user).values_list('event', flat=True)
    user_events = Event.objects.filter(pk__in=event_keys).values_list('eventid', 'name')
    form = PhotoForm(user_events)

    if request.method == "POST":
        form = PhotoForm(user_events, request.POST, request.FILES)
        if form.is_valid():
            content_parent = Content(content_ref=Event.objects.get(pk=request.POST['atevent']))
            content_parent.save()
            new_form = form.save(commit=False)
            new_form.createdby = request.user
            new_form.creationtime = timezone.now()
            new_form.id = content_parent
            new_form.save()
            return redirect('socnet:user_photos', identifier=request.user.identifier)

    return render(request, 'socnet/upload_photo.html', {'form': form})
    
    
@login_required(login_url='login')
def send_message(request):
    results_list = []
    friend_ids = Friendship.objects.filter(usr1=request.user).filter(pending=False).values_list('usr2__identifier', flat=True).union(
                    Friendship.objects.filter(usr2=request.user).filter(pending=False).values_list('usr1__identifier', flat=True))
    friends = get_user_list(identifier__in=friend_ids).values_list('identifier', 'first_name', 'last_name')

    for f in friends:
        results_list.append((f[0], f[1] + ' ' +  f[2]))

    if request.method == 'POST':
        msg_form = NewMessageForm(results_list, request.POST)
        if msg_form.is_valid():
            new_msg_form = msg_form.save(commit=False)
            new_msg_form.senton = timezone.now()
            new_msg_form.sender = request.user

            receivers = request.POST.getlist('receiver')
            participants = receivers + [request.user.identifier]
            common_convs = Usrconversation.objects.all().values_list('conversation', flat=True)
            for p in participants:
                common_convs = common_convs.intersection(Usrconversation.objects.filter(usr__identifier=p).values_list('conversation', flat=True))

            participants_objs = get_user_list(identifier__in=participants)
            for conv in common_convs:
                if len(Usrconversation.objects.filter(conversation__convid=conv)) == len(participants_objs):
                    new_msg_form.partof = Conversation.objects.get(pk=conv)
                    break
            else:
                new_conv = Conversation(name=(', '.join([p.first_name for p in participants_objs[:3]]) + ("..." if len(participants_objs) > 3 else "")), beginning=timezone.now())
                new_conv.save()
                new_msg_form.partof = new_conv

                for p in participants_objs:
                    conv_join = Usrconversation(usr=p, conversation=new_msg_form.partof)
                    conv_join.save()

            new_msg_form.save()
            return redirect('socnet:conversation_detail', convid=new_msg_form.partof.convid)
    else:
        msg_form = NewMessageForm(results_list)
    
    return render(request, 'socnet/new_message.html', {'form': msg_form})

@login_required(login_url='login')
def conversation_view(request):
    conversation_list = Usrconversation.objects.filter(usr=request.user)
    
    return render(request, 'socnet/conversation_view.html', {'conversation_list': conversation_list})
    
    
@login_required(login_url='login') 
def conversation_detail(request, convid):
    usr_convs = Usrconversation.objects.filter(usr__identifier=request.user.identifier)

    conv_messages = Message.objects.filter(partof__convid=convid)
    
    if request.method == 'POST':
        submitted = ConversationForm(request.POST)
        if submitted.is_valid():
            conversation = Conversation.objects.get(convid=convid)
            new_msg = submitted.save(commit=False)
            new_msg.senton = date.today()
            new_msg.sender = request.user
            new_msg.partof = conversation
            new_msg.save()
            return redirect('socnet:conversation_detail', convid=convid)
    else:
        submitted = ConversationForm()
    
    return render(request, 'socnet/conversation_detail.html', {'conv_messages': conv_messages, 'form': submitted})

def publish_post(request):
    event_keys = Usrevent.objects.filter(usr=request.user).values_list('event', flat=True)
    user_events = Event.objects.filter(pk__in=event_keys).values_list('eventid', 'name')

    if request.method == "POST":
        form = PostForm(user_events, request.POST, request.FILES)
        if form.is_valid():
            content_parent = Content(content_ref=Event.objects.get(pk=request.POST['atevent']))
            content_parent.save()
            new_form = form.save(commit=False)
            new_form.createdby = request.user
            new_form.creationtime = timezone.now()
            new_form.id = content_parent
            new_form.name = "PostyPost"
            new_form.save()
            messages.success(request, 'Your post has been published!')

    return redirect('socnet:index')

def publish_post_from_profile(request):
    event_keys = Usrevent.objects.filter(usr=request.user).values_list('event', flat=True)
    user_events = Event.objects.filter(pk__in=event_keys).values_list('eventid', 'name')

    if request.method == "POST":
        form = PostForm(user_events, request.POST, request.FILES)
        if form.is_valid():
            content_parent = Content(content_ref=Event.objects.get(pk=request.POST['atevent']))
            content_parent.save()
            new_form = form.save(commit=False)
            new_form.createdby = request.user
            new_form.creationtime = timezone.now()
            new_form.id = content_parent
            new_form.name = "PostyPost"
            new_form.save()
            messages.success(request, 'Your post has been published!')

    return redirect('socnet:view_profile', identifier=request.user.identifier)

def publish_post_to_event(request, eventid):
    if request.method == "POST":
        form = PostToEventForm(request.POST, request.FILES)
        if form.is_valid():
            content_parent = Content(content_ref=Event.objects.get(pk=eventid))
            content_parent.save()
            new_form = form.save(commit=False)
            new_form.createdby = request.user
            new_form.creationtime = timezone.now()
            new_form.id = content_parent
            new_form.name = "PostyPost"
            new_form.save()
            messages.success(request, 'Your post has been published!')

    return redirect('socnet:event_detail', eventid=eventid)

@login_required(login_url='login')
def user_events(request, identifier):
    user = get_user(identifier)
    form = BBUserViewForm(instance=user)
    profile_pic = user.get_profile_pic()

    usr1 = request.user
    usr2 = user
    going = Usrevent.objects.filter(usr__identifier=identifier).values_list('event', flat=True)
    is_going = Event.objects.filter(pk__in=going, eventtime__gte=timezone.now())
    has_gone = Event.objects.filter(pk__in=going, eventtime__lt=timezone.now())
    organising = Event.objects.filter(createdby__identifier=identifier, eventtime__gte=timezone.now())
    has_organised = Event.objects.filter(createdby__identifier=identifier, eventtime__lt=timezone.now())
    context = {'form': form, 'profile_pic': profile_pic, 'is_going': is_going, 'has_gone': has_gone,
               'organising': organising, 'has_organised': has_organised}
    if request.user != user:
        if Relationship.objects.filter(usr1__exact=usr1) & Relationship.objects.filter(usr2__exact=usr2):
            relationship = 'subscribed'
        else:
            relationship = 'notsubscribed'
        friendship = None
        if (Friendship.objects.filter(usr1__exact=usr1).filter(usr2__exact=usr2) or
                Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=False)):
            friendship = 'friends'
        elif Friendship.objects.filter(usr1__exact=usr2).filter(usr2__exact=usr1).filter(pending=True):
            friendship = 'requested'
        context = {**context, 'relationship': relationship, 'friendship': friendship}

    return render(request, 'socnet/view_user_events.html', context)


@login_required(login_url='login')
def search_result(request):
    users_list = []
    names = []
    if request.method == 'POST' and request.POST.get('search_box') is not None:
        if request.POST.get('search_box') != '' and len(request.POST.get('search_box').split(' ')) == 1:
            for result in get_user_list(identifier__startswith=normalize_string(request.POST.get('search_box'))):
                users_list.append(result)
        if request.POST.get('search_box') != '':
            search_strings = normalize_name(request.POST.get('search_box'))
            for string in search_strings:
                names.append(name_to_identifier(string['first_name'], string['last_name']))
            for name in names:
                for result in get_user_list(identifier__contains=name):
                    users_list.append(result)
        else:
            users_list = get_user_list()
    else:
        users_list = get_user_list()

    return render(request, 'socnet/search_result.html', {'user_list': set(users_list)})