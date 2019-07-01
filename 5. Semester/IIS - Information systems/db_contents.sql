-- Name: IIS Information System Project 2018 - Database schema creation
-- Subject: Information Systems 2017/2018
-- Institute: Faculty of Information Technology, Brno University of Technology
--
-- Original for Database systems project in summer term 2018.
-- Rewritten from Oracle PL/SQL in PostgreSQL syntax in order to facilitate
-- information system project needs.
-- Procedures, explain plain and privileges granting from original file is
-- omitted in this file, since there is no need for them for our use case.
--
-- Author: xgolds00 AT stud.fit.vutbr.cz
-- Author: xhamra00 AT stud.fit.vutbr.cz
--
-- Date: April 2018, last review on November 2018

-- Drop constraint preventing from table dropping.
ALTER TABLE IF EXISTS Album
DROP CONSTRAINT FK_AlbumTitlePhoto;

-- Remove tables with the same name that already exist in the database.
DROP TABLE IF EXISTS UsrPhoto;
DROP TABLE IF EXISTS Photo;
DROP TABLE IF EXISTS PostKeyword;
DROP TABLE IF EXISTS Keyword;
DROP TABLE IF EXISTS UsrPost;
DROP TABLE IF EXISTS Post;
DROP TABLE IF EXISTS UsrEvent;
DROP TABLE IF EXISTS Event;
DROP TABLE IF EXISTS Album;
DROP TABLE IF EXISTS Commentary;
DROP TABLE IF EXISTS Content;
DROP TABLE IF EXISTS Message;
DROP TABLE IF EXISTS UsrConversation;
DROP TABLE IF EXISTS Conversation;
DROP TABLE IF EXISTS Relationship;
DROP TABLE IF EXISTS Usr;

-- Create table User.  Abbreviated to "Usr", since User is a SQL language
-- keyword and we do not want to use it quoted all the time.
CREATE TABLE Usr (
   UsrID SERIAL PRIMARY KEY,
   Name VARCHAR(32) NOT NULL,
   Surname VARCHAR(32) NOT NULL,
   Gender VARCHAR(11) NOT NULL,
   Birthdate DATE NOT NULL,
   Birthplace VARCHAR(32),
   Residence VARCHAR(32),
   FamilyStatus VARCHAR(15),
   Education VARCHAR(32),
   Workplace VARCHAR(32),
   Email VARCHAR(32) NOT NULL,
   
   CONSTRAINT checkFStat CHECK (FamilyStatus IN ('single', 'married',
                                                 'divorced', 'widowed',
                                                 'in relationship')),
   CONSTRAINT checkGender CHECK (Gender IN ('male', 'female', 'unspecified'))
);

-- Relationship between two users.  Attribute entity.
CREATE TABLE Relationship (
   RelationshipType VARCHAR(8) NOT NULL,
   StartDate DATE NOT NULL,
   Usr1 INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Usr2 INTEGER REFERENCES Usr(UsrID) NOT NULL,
   CONSTRAINT PK_Rel PRIMARY KEY (Usr1, Usr2),
   CONSTRAINT checkRelType CHECK (RelationshipType IN ('friend', 'family',
                                                       'coworker'))
);

-- Entity conversation.
CREATE TABLE Conversation (
   ConvID SERIAL PRIMARY KEY,
   Name VARCHAR(32) NOT NULL,
   Beginning TIMESTAMP NOT NULL
);

-- Represents relationship between 2...N users and M conversations.
-- Relationship is represented using intersection table as in M to N
-- relationships and constraint of 2 or more users has to be controlled on
-- the application level.
CREATE TABLE UsrConversation (
   Usr INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Conversation INTEGER REFERENCES Conversation(ConvID) NOT NULL,
   
   CONSTRAINT PK_UsrConv PRIMARY KEY (Usr, Conversation)
);

-- Message in the conversation representation.
-- Weak entity linked to Conversation table.
CREATE TABLE Message (
   MsgOrder SERIAL NOT NULL,
   Subject VARCHAR(32),
   Contents VARCHAR(512) NOT NULL,
   SentOn TIMESTAMP NOT NULL,
   Place VARCHAR(32),
   Sender INTEGER REFERENCES Usr(UsrID) NOT NULL,
   PartOf INTEGER NOT NULL,
   
   CONSTRAINT FK_MessageWeak FOREIGN KEY (PartOf) REFERENCES Conversation(ConvID)
              ON DELETE CASCADE,
   CONSTRAINT PK_Message PRIMARY KEY (MsgOrder, PartOf)
);

-- Generalized entity content - used for generating unique identifier for other
-- content-like entities.
-- This Schema was chosen to keep "simplicity" of database, where entity
-- Commentary is tied to content.  If we decided to split content parts
-- completely, separate entities like CommentAlbum, CommentPhoto etc. would
-- have to be created.  This way, commentary is tied with Content ID, and other
-- entities derived from this take its ID and use it in its own table and data.
-- This way, we actually need to insert data into 2 tables when creating new
-- derivative of Content entity.  To provide acceptable interface and keep user
-- convenience at maximum, we created INSERT triggers which are able to modify
-- both tables in only one INSERT call.  However, triggers have problem with
-- creating initial ContentID for value "1" using automatically-generated
-- values, because the 1st inserted always started with "2" for unknown
-- reasons.  We hacked this behavior with simple IF statement in triggers and
-- selecting initial numbering start value as "2".
CREATE TABLE Content (
   ContentID SERIAL PRIMARY KEY
);

-- Entity comment - renamed to commentary because of collision with "comment"
-- SQL keyword.
-- Weak entity linked to content.
CREATE TABLE Commentary (
   CommentOrder SERIAL NOT NULL,
   Contents VARCHAR(512) NOT NULL,
   SentOn TIMESTAMP NOT NULL,
   Place VARCHAR(32),
   Sender INTEGER REFERENCES Usr(UsrID) NOT NULL,
   ContainedIn INTEGER NOT NULL,
   
   CONSTRAINT FK_WeakCommentWeak FOREIGN KEY (ContainedIn) REFERENCES
              Content(ContentID) ON DELETE CASCADE,
   CONSTRAINT PK_Commentary PRIMARY KEY (CommentOrder, ContainedIn)
);

-- Entity album.
-- Use procedure InsertAlbum instead of INSERT when adding new elements.
CREATE TABLE Album (
   AlbumID INTEGER REFERENCES Content(ContentID) NOT NULL,
   Name VARCHAR(32) NOT NULL,
   Description VARCHAR(256),
   CreationTime TIMESTAMP NOT NULL,
   CreationPlace VARCHAR(32),
   CreatedBy INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Privacy CHAR(9) NOT NULL,
    
   CONSTRAINT PK_Album PRIMARY KEY (AlbumID),
   CONSTRAINT CheckPrivacy CHECK (Privacy IN ('private', 'public', 'protected'))
);

-- Entity event.
-- Use procedure InsertEvent instead of INSERT when adding new elements.
CREATE TABLE Event (
   EventID INTEGER REFERENCES Content(ContentID) NOT NULL,
   Name VARCHAR(32) NOT NULL,
   Description VARCHAR(256),
   CreationTime TIMESTAMP NOT NULL,
   CreationPlace VARCHAR(32),
   CreatedBy INTEGER REFERENCES Usr(UsrID) NOT NULL,
   EventLocation VARCHAR(64) NOT NULL,
   EventTime TIMESTAMP NOT NULL,
   
   CONSTRAINT PK_Event PRIMARY KEY (EventID)
);

-- Intersection table represents M:N relationship between user and event.
-- User attends N events, but single event can be attended by M people.
CREATE TABLE UsrEvent (
   Usr INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Event INTEGER REFERENCES Event(EventID) NOT NULL,
   
   CONSTRAINT PK_UsrEvent PRIMARY KEY (Usr, Event)
);

-- Entity post.
-- Use procedure InsertPost instead of INSERT when adding new elements.
CREATE TABLE Post (
   PostID INTEGER REFERENCES Content(ContentID) NOT NULL,
   Name VARCHAR(32) NOT NULL,
   Description VARCHAR(256),
   CreationTime TIMESTAMP NOT NULL,
   CreationPlace VARCHAR(32),
   CreatedBy INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Contents VARCHAR(512) NOT NULL,
   
   CONSTRAINT PK_Post PRIMARY KEY (PostID)
);

-- Intersection table represents M:N relationship between user and post.
-- User is tagged on 0...N posts and single post tags 0...M users.
CREATE TABLE UsrPost (
   Usr INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Post INTEGER REFERENCES Post(PostID) NOT NULL,
   
   CONSTRAINT PK_UsrPost PRIMARY KEY(Usr, Post)
);

-- Entity keyword posing as a "tag" to be linked with posts.
-- ID is being used as a primary key, since having primary key keyword would
-- mean, that PostKeyword table would have to store more data in the database.
-- This way, database is more compact in the cost of lesser user convenience.
CREATE TABLE Keyword (
   KeywordID SERIAL PRIMARY KEY,
   Keyword VARCHAR(32) NOT NULL
);

-- Intersection table to represent M:N between entity Post and Keyword.
-- Post can contain N Keywords, but Keyword can be contained in M posts.
CREATE TABLE PostKeyword (
   Post INTEGER REFERENCES Post(PostID) NOT NULL,
   Keyword INTEGER REFERENCES Keyword(KeywordID) NOT NULL,
   
   CONSTRAINT PK_PostKeyword PRIMARY KEY (Post, Keyword)
);

-- Entity post.
-- Use procedure InsertPhoto instead of INSERT when adding new elements.
CREATE TABLE Photo (
   PhotoID INTEGER REFERENCES Content(ContentID) NOT NULL,
   Name VARCHAR(32) NOT NULL,
   Description VARCHAR(256),
   CreationTime TIMESTAMP NOT NULL,
   CreationPlace VARCHAR(32),
   CreatedBy INTEGER REFERENCES Usr(UsrID) NOT NULL,
   CapturePlace VARCHAR(32),
   CaptureDate DATE,
   BelongsTo INTEGER REFERENCES Album(AlbumID),
   
   CONSTRAINT PK_Photo PRIMARY KEY (PhotoID)
);

-- Intersection table represents M:N relationship between user and post.
-- User is tagged on 0...N photos and single photo tags 0...M users.
CREATE TABLE UsrPhoto (
   Usr INTEGER REFERENCES Usr(UsrID) NOT NULL,
   Photo INTEGER REFERENCES Photo(PhotoID) NOT NULL,
   
   CONSTRAINT PK_UsrPhoto PRIMARY KEY(Usr, Photo)
);

-- Add Title photo attribute to the Album table and add constraint, that only
-- photo contained in the table can be set as title.
ALTER TABLE Album
ADD TitlePhoto INTEGER;

ALTER TABLE Album
ADD CONSTRAINT FK_AlbumTitlePhoto FOREIGN KEY (TitlePhoto) REFERENCES
Photo(PhotoID);

-------------------------------------------------------------------------------
------------------------------  TRIGGERS SECTION  -----------------------------
-------------------------------------------------------------------------------
-- Triggers for keeping data consistency between entity Content and its
-- derivatives.

-- Function to make a insert to the Content table and return inserted ID
CREATE OR REPLACE FUNCTION NewContent()
   RETURNS INTEGER AS
$newcontent$
DECLARE
   NewID INTEGER;
BEGIN 
   -- Make insert to Content table first
   INSERT INTO Content VALUES(DEFAULT);

   -- Pick the just-inserted row and return it
   SELECT C.ContentID INTO NewID FROM Content C
   ORDER BY C.ContentID DESC
   LIMIT 1;

   RETURN NewID;
END;
$newcontent$ LANGUAGE plpgsql;

-- Album INSERT trigger.
CREATE OR REPLACE FUNCTION AlbumInserter()
   RETURNS TRIGGER AS
$AlbumInsertTrig$
BEGIN
   -- Add new row to the Contents table and use its ID for new Album entry
   NEW.AlbumID := NewContent();
   RETURN NEW;
END;
$AlbumInsertTrig$ LANGUAGE plpgsql;

CREATE TRIGGER AlbumInsertTrig BEFORE INSERT ON Album FOR EACH ROW
   EXECUTE PROCEDURE AlbumInserter();


-- Event INSERT trigger.
CREATE OR REPLACE FUNCTION EventInserter()
   RETURNS TRIGGER AS
$EventInsertTrig$
BEGIN
   -- Add new row to the Contents table and use its ID for new Event entry
   NEW.EventID := NewContent();
   RETURN NEW;
END;
$EventInsertTrig$ LANGUAGE plpgsql;

CREATE TRIGGER EventInsertTrig BEFORE INSERT ON Event FOR EACH ROW
   EXECUTE PROCEDURE EventInserter();


-- Post INSERT trigger.
CREATE OR REPLACE FUNCTION PostInserter()
   RETURNS TRIGGER AS
$EventInsertTrig$
BEGIN
   -- Add new row to the Contents table and use its ID for new Event entry
   NEW.PostID := NewContent();
   RETURN NEW;
END;
$EventInsertTrig$ LANGUAGE plpgsql;

CREATE TRIGGER PostInsertTrig BEFORE INSERT ON Post FOR EACH ROW
   EXECUTE PROCEDURE PostInserter();


-- Photo INSERT trigger.
CREATE OR REPLACE FUNCTION PhotoInserter()
   RETURNS TRIGGER AS
$PhotoInsertTrig$
BEGIN
   -- Add new row to the Contents table and use its ID for new Event entry
   NEW.PhotoID := NewContent();
   RETURN NEW;
END;
$PhotoInsertTrig$ LANGUAGE plpgsql;

CREATE TRIGGER PhotoInsertTrig BEFORE INSERT ON Photo FOR EACH ROW
   EXECUTE PROCEDURE PhotoInserter();


-- Triggers that synchronize content table on delete of one of its derivatives.
-- Album DELETE trigger.
CREATE OR REPLACE FUNCTION AlbumDeleter()
   RETURNS TRIGGER AS 
$AlbumDeleteTrig$
BEGIN
   DELETE FROM Content
   WHERE ContentID = OLD.AlbumID;

   RETURN OLD;
END;
$AlbumDeleteTrig$ LANGUAGE plpgsql;

CREATE TRIGGER AlbumDeleteTrig AFTER DELETE ON Album FOR EACH ROW
   EXECUTE PROCEDURE AlbumDeleter();


-- Event DELETE trigger.
CREATE OR REPLACE FUNCTION EventDeleter()
   RETURNS TRIGGER AS
$EventDeleteTrig$   
BEGIN
   DELETE FROM Content
   WHERE ContentID = OLD.EventID;

   RETURN OLD;
END;
$EventDeleteTrig$ LANGUAGE plpgsql;

CREATE TRIGGER EventDeleteTrig AFTER DELETE ON Event FOR EACH ROW
   EXECUTE PROCEDURE EventDeleter();


-- Post DELETE trigger.
CREATE OR REPLACE FUNCTION PostDeleter()
   RETURNS TRIGGER AS
$PostDeleteTrig$
BEGIN
   DELETE FROM Content
   WHERE ContentID = OLD.PostID;

   RETURN OLD;
END;
$PostDeleteTrig$ LANGUAGE plpgsql;

CREATE TRIGGER PostDeleteTrig AFTER DELETE ON Post FOR EACH ROW
   EXECUTE PROCEDURE PostDeleter();


-- Photo DELETE trigger.
CREATE OR REPLACE FUNCTION PhotoDeleter()
   RETURNS TRIGGER AS
$PhotoDeleteTrig$
BEGIN
   DELETE FROM Content
   WHERE ContentID = OLD.PhotoID;

   RETURN OLD;
END;
$PhotoDeleteTrig$ LANGUAGE plpgsql;

CREATE TRIGGER PhotoDeleteTrig AFTER DELETE ON Photo FOR EACH ROW
   EXECUTE PROCEDURE PhotoDeleter();

-- Our initial database design is supposed to track message and comments order
-- for them to be easily accessible without further computational resources.
-- Thus, we need to create triggers to handle unique numbering for every one
-- of these entities.  This costs us a little more computational resources
-- during INSERT operation, but searching for concrete entry is much simpler.
-- These queries are not optimized for now due to time pressure for other
-- projects, we may optimize it later in the process.

CREATE OR REPLACE FUNCTION MessageOrderer()
   RETURNS TRIGGER AS
$MessageOrderTrig$
DECLARE
   maxVal INTEGER;
BEGIN
   -- Select last message order number in the current conversation
   SELECT MAX(M.MsgOrder) INTO maxVal
   FROM Message M
   WHERE M.PartOf = NEW.PartOf;

   -- Check if this is not first message in the conversation
   IF maxVal IS NULL THEN
      maxVal := 0;
   END IF;

   -- Increment MsgOrder by 1
   NEW.MsgOrder := maxVal + 1;
   
   RETURN NEW;
END;
$MessageOrderTrig$ LANGUAGE plpgsql;

CREATE TRIGGER MessageOrderTrig BEFORE INSERT ON Message FOR EACH ROW
   EXECUTE PROCEDURE MessageOrderer();


CREATE OR REPLACE FUNCTION CommentaryOrderer()
   RETURNS TRIGGER AS
$CommentaryOrderTrig$
DECLARE
   maxVal INTEGER;
BEGIN
   -- Find CommentOrder maximum on the current post
   SELECT MAX(C.CommentOrder) INTO maxVal
   FROM Commentary C
   WHERE C.ContainedIn = NEW.ContainedIn;
   
   -- Check if this is not first message in the conversation
   IF maxVal IS NULL THEN
      maxVal := 0;
   END IF;
   
   -- Increment MsgOrder by 1
   NEW.CommentOrder := maxVal + 1;
   
   RETURN NEW;
END;
$CommentaryOrderTrig$ LANGUAGE plpgsql;

CREATE TRIGGER CommentaryOrderTrig BEFORE INSERT ON Commentary FOR EACH ROW
   EXECUTE PROCEDURE CommentaryOrderer();
   

-- Checks that title photo belongs to the album
CREATE OR REPLACE FUNCTION AlbumTitleGuard()
   RETURNS TRIGGER AS
$AlbumTitleGuardTrig$
DECLARE
   PhotoExistence INTEGER;
BEGIN
   -- Subselect statement returns all Photo IDs, that are contained in the
   -- album row user is trying to create/modify. Then, main select compares
   -- found results with the desired new title photo ID and saves 0 or 1 to the
   -- variable, signaling if the new title photo is in the album or not.
   SELECT COUNT(*) INTO PhotoExistence
      FROM Photo 
      WHERE NEW.TitlePhoto IN(SELECT P.PhotoID
                               FROM Photo P
                               WHERE P.BelongsTo = NEW.AlbumID);
   -- If the new Title photo is not NULL, checks if it exist in the album
   -- according to the previous select statement.
   IF (NEW.TitlePhoto IS NOT NULL and PhotoExistence = 0) THEN
       RAISE EXCEPTION USING MESSAGE = 'Photo can not be set as a title ' ||
       'because it is not contained in the album.';
   END IF;
   
   RETURN NEW;
END;
$AlbumTitleGuardTrig$ LANGUAGE plpgsql;

CREATE TRIGGER AlbumTitleGuardInsertTrig
BEFORE INSERT ON Album FOR EACH ROW
   EXECUTE PROCEDURE AlbumTitleGuard();
   
CREATE TRIGGER AlbumTitleGuardUpdateTrig
BEFORE UPDATE OF TitlePhoto ON Album FOR EACH ROW
   EXECUTE PROCEDURE AlbumTitleGuard();

--------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------
------------------------------  INSERTS SECTION  ------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
------------------------------        USR        ------------------------------
-------------------------------------------------------------------------------
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email) 
         Values('Patrik', 'Goldschmidt', 'male', TO_TIMESTAMP('23-05-1996',
         'DD-MM-YYYY') ,'Nove Zamky', 'Nove Zamky', 'single', 
         'VUT FIT BRNO', 'UPSY VUT FIT Brno', 'xgolds00@stud.fit.vutbr.cz');
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email) 
         Values('Peter', 'Hamran', 'male', TO_TIMESTAMP('05-12-1996',
         'DD-MM-YYYY') ,'Komarno', 'Dulovce', 'single', 
         'VUT FIT BRNO', 'MoraviaIT sro.', 'xhamra00@stud.fit.vutbr.cz');
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email) 
         Values('Nikolas', 'Kassulke', 'male', TO_TIMESTAMP('22-09-2014',
         'DD-MM-YYYY') ,'East Duncan', 'Eveline', 'single', 
         'FIT MUNI', 'Wanda', 'Mollie@gia.net');
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Providenci', 'Kub', 'unspecified', TO_TIMESTAMP('16-06-1997',
         'DD-MM-YYYY'), 'Gerlachmouth', 'Rosamond', 'married', 'SPSE NZ', 
         'Maureen AS', 'Jaquan_Langosh@russell.io'); 
      
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Kariane', 'Schroeder', 'female', TO_TIMESTAMP('13-08-1999',
         'DD-MM-YYYY'), 'Lake Elias', 'Timmy', 'divorced', 
         'University of Transport', 'Eusebio', 'Mckenzie.Sanford@monroe.ca');
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Winifred', 'Hintz', 'male', TO_TIMESTAMP('02-11-2000',
         'DD-MM-YYYY'), 'Cummerataton', 'Kane', 'widowed', 
         'Comenius University', 'None', 'Brady@charlotte.com'); 
            
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Chloe', 'Boyle', 'female', TO_TIMESTAMP('01-06-1987',
         'DD-MM-YYYY'), 'Lake Asa', 'Wilson', 'married', 
         'Matej Bel University', 'Verla', 'Richmond.Shanahan@nicole.biz'); 
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Domenic', 'Corwin', 'female', TO_TIMESTAMP('21-09-1985',
         'DD-MM-YYYY'), 'West Hollie', 'Candice', 'married', 
         'Elizabethan University', 'Alvena', 'Tyreek@antoinette.io'); 
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Aiden', 'Kuvalis', 'female', TO_TIMESTAMP('22-03-2015',
         'DD-MM-YYYY'), 'Vandervortville', 'Mireille', 'single', 
         'Nursery school of Dulovce', 'None', 'America@litzy.co.uk'); 
         
INSERT INTO Usr (Name, Surname, Gender, Birthdate, Birthplace, Residence, 
             FamilyStatus, Education, Workplace, Email)        
         VALUES('Aiden', 'Kuvalis', 'female', TO_TIMESTAMP('21-03-1975',
         'DD-MM-YYYY'), 'Bertrandville', 'Yasmeen', 'widowed', 
         ' College of Agricultural', 'Alvena', 
         'Cayla.Balistreri@amparo.com'); 

-------------------------------------------------------------------------------
------------------------------   RELATIONSHIP   -------------------------------
-------------------------------------------------------------------------------     
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('friend', 
         TO_TIMESTAMP('21-03-2012','DD-MM-YYYY'),1, 2);
   
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('friend', 
         TO_TIMESTAMP('24-07-2012','DD-MM-YYYY'),1, 3);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('friend', 
         TO_TIMESTAMP('21-03-2012','DD-MM-YYYY'),2, 3);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('friend', 
         TO_TIMESTAMP('24-11-2013','DD-MM-YYYY'),2, 5);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2)  
         VALUES('friend', 
         TO_TIMESTAMP('12-12-2013','DD-MM-YYYY'),3, 7);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('family', 
         TO_TIMESTAMP('08-07-2013','DD-MM-YYYY'),4, 8);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('family', 
         TO_TIMESTAMP('19-05-2014','DD-MM-YYYY'),5, 6);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('coworker', 
         TO_TIMESTAMP('07-09-2015','DD-MM-YYYY'),5, 7);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('coworker', 
         TO_TIMESTAMP('22-03-2015','DD-MM-YYYY'),6, 8);
         
INSERT INTO Relationship (RelationshipType, StartDate, Usr1, Usr2) 
         VALUES('coworker', 
         TO_TIMESTAMP('30-10-2017','DD-MM-YYYY'),8, 9);

-------------------------------------------------------------------------------
------------------------------   CONVERSATION   -------------------------------
-------------------------------------------------------------------------------
INSERT INTO Conversation (Name, Beginning) VALUES('Party hrad', 
         TO_TIMESTAMP('10-05-2014 20:10:10','DD-MM-YYYY HH24:MI:SS'));
         
INSERT INTO Conversation (Name, Beginning) VALUES('VUT FIT BIT', 
         TO_TIMESTAMP('07-02-2016 21:00:00','DD-MM-YYYY HH24:MI:SS'));
         
INSERT INTO Conversation (Name, Beginning) VALUES('Novozamocka liga', 
         TO_TIMESTAMP('01-01-1995 00:00:00','DD-MM-YYYY HH24:MI:SS'));
         
INSERT INTO Conversation (Name, Beginning) VALUES('Brankarske forum', 
         TO_TIMESTAMP('31-12-2015 23:59:10','DD-MM-YYYY HH24:MI:SS'));
         
INSERT INTO Conversation (Name, Beginning) VALUES('Girls just wanna have fun', 
         TO_TIMESTAMP('10-05-2014 20:10:10','DD-MM-YYYY HH24:MI:SS'));
         
INSERT INTO Conversation (Name, Beginning) VALUES('Party hrad', 
         TO_TIMESTAMP('09-04-1997 13:09:11','DD-MM-YYYY HH24:MI:SS'));
         
INSERT INTO Conversation (Name, Beginning) VALUES('Imber est', 
         TO_TIMESTAMP('13-09-2011 03:04:05','DD-MM-YYYY HH24:MI:SS'));

-------------------------------------------------------------------------------
------------------------------ USRCONVERSATION  -------------------------------
-------------------------------------------------------------------------------
INSERT INTO UsrConversation (Usr, Conversation) VALUES(1, 1);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(1, 2);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(2, 1);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(2, 3);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(2, 4);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(3, 2);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(3, 4);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(4, 2);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(4, 5);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(5, 5);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(6, 5);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(6, 6);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(7, 6);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(7, 3);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(8, 7);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(9, 7);
INSERT INTO UsrConversation (Usr, Conversation) VALUES(9, 5);

-------------------------------------------------------------------------------
------------------------------      MESSAGE     -------------------------------
-------------------------------------------------------------------------------
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('Obama', 'Is President Obama going to finally mention the wo
               rds radical Islamic terrorism? If he doesnt he should imm
               ediately resign in disgrace!', 
         TO_TIMESTAMP('13-09-2011 03:04:05','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 1, 1);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('LGBT', 'Thank you to the LGBT community! I will fight for y
               ou while Hillary brings in more people that will threaten y
               our freedoms and beliefs', 
         TO_TIMESTAMP('14-09-2012 05:06:05','DD-MM-YYYY HH24:MI:SS'),
         'Olomouc', 2, 1);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('Abandoned by Demis', 'DACA was abandoned by the Democrats. 
               Very unfair to them! Would have been tied to desperately n
               eeded Wall.', 
         TO_TIMESTAMP('14-09-2012 07:12:20','DD-MM-YYYY HH24:MI:SS'),
         'Olomouc', 2, 3);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('Debt', 'Our $17T national debt and $1T yearly budget defic
               its are a national security risk of the highest order.', 
         TO_TIMESTAMP('21-12-2013 22:23:24','DD-MM-YYYY HH24:MI:SS'),
         'Praha', 2, 3);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('great hero', 'Officer died after bravely swapping places w
               ith hostage in ISIS related terror attack.', 
         TO_TIMESTAMP('22-12-2013 22:23:24','DD-MM-YYYY HH24:MI:SS'),
         'Vitkovice', 3, 2);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES(NULL, 'I will NEVER sign another bill like this again. To 
                    prevent this omnibus situation from ever happening
                    again, Im calling on Congress to give me a line-it
                    em veto for all govt spending bills!', 
         TO_TIMESTAMP('08-11-2014 11:05:07','DD-MM-YYYY HH24:MI:SS'),
         'Ceske Budejovice', 3, 4);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('Obama', 'Obama Administration legalized bump stocks. BAD I
               DEA. As I promised, today the Department of Justice will i
               ssue the rule banning BUMP STOCKS with a mandated comment 
               period.', 
         TO_TIMESTAMP('11-11-2014 12:04:07','DD-MM-YYYY HH24:MI:SS'),
         NULL, 4, 2);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf)  
         VALUES('White House', 'News conference at the White House concerni
               ng the Omnibus Spending Bill.', 
         TO_TIMESTAMP('13-11-2014 17:13:20','DD-MM-YYYY HH24:MI:SS'),
         'Prostejov', 4, 5);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('VETO', 'I am considering a VETO of the Omnibus Spending Bi
                     ll based on the fact that the 800,000 plus DACA re
                     cipients have been totally abandoned by the Democr
                     ats and the BORDER WALL.', 
         TO_TIMESTAMP('01-12-2014 12:12:21','DD-MM-YYYY HH24:MI:SS'),
         'Vyskov', 5, 5);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf)  
         VALUES('Intelligence Committee', 'House Intelligence Committee vot
               es to release final report. FINDINGS: (1) No evidence prov
               ided of Collusion between Trump Campaign Russia.', 
         TO_TIMESTAMP('12-12-2014 21:02:07','DD-MM-YYYY HH24:MI:SS'),
         'Plzen', 6, 5);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('@AmbJohnBolton', 'I am pleased to announce that, effectiv
               e 4/9/18, @AmbJohnBolton will be my new National Security
               Advisor.', 
         TO_TIMESTAMP('13-09-2015 03:04:05','DD-MM-YYYY HH24:MI:SS'),
         'Rosice', 6, 5);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf) 
         VALUES('honor ', 'My honor Charlie - thank you! #MAGA', 
         TO_TIMESTAMP('21-11-2015 04:13:20','DD-MM-YYYY HH24:MI:SS'),
         'Kromeriz', 6, 6);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf)  
         VALUES('I pledged', 'As a candidate, I pledged that if elected I w
               ould use every lawful tool to combat unfair trade, protect
               American workers, and defend our national security.', 
         TO_TIMESTAMP('21-11-2015 4:13:20','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 7, 6);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf)  
         VALUES('Money', 'Got $1.6 Billion to start Wall on Southern Border
               , rest will be forthcoming. Most importantly, got $700 Bil
               lion to rebuild our Military, $716 Billion next year', 
         TO_TIMESTAMP('22-02-2017 03:04:05','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 8, 7);
         
INSERT INTO Message (Subject, Contents, SentOn, Place, Sender, PartOf)  
         VALUES('STRENGTH!', '.They can help solve problems with North Kore
               a, Syria, Ukraine, ISIS, Iran and even the coming Arms Rac
               e.', 
         TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Praha', 9, 7);
   
-------------------------------------------------------------------------------
---------------------------------    ALBUM   ----------------------------------
-------------------------------------------------------------------------------
INSERT INTO Album (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Privacy) VALUES('Albaac', 'Ten nejlepsi album',
            TO_TIMESTAMP('25-03-2018', 'DD-MM-YYYY'), 'Brno', 1, 'private');

INSERT INTO Album (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Privacy) VALUES('Moje Fotecky', '',
            TO_TIMESTAMP('14-01-2018', 'DD-MM-YYYY'), 'Ostrava', 2, 'public');

INSERT INTO Album (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Privacy) VALUES('Rybarna', 'Jindra sel na ryby',
            TO_TIMESTAMP('01-09-2017', 'DD-MM-YYYY'), 'Dira u Brna', 5,
            'public');

INSERT INTO Album (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Privacy) VALUES('Selfiesv1', 'Proste jenom ja cmuQ cmuQ',
            TO_TIMESTAMP('24-04-2017', 'DD-MM-YYYY'), 'Praha', 7, 'public');

INSERT INTO Album (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Privacy) VALUES('Selfiesv2', 'Ja uz zase cmuQy cmuQy',
            TO_TIMESTAMP('30-04-2017', 'DD-MM-YYYY'), 'Praha', 7, 'public');

-------------------------------------------------------------------------------
---------------------------------    EVENT   ----------------------------------
-------------------------------------------------------------------------------
INSERT INTO Event (Name, Description, CreationTime, CreationPlace, CreatedBy,
            EventLocation, EventTime) VALUES('Dance dance party party',
            'Will be dancin until mornin',
         TO_TIMESTAMP('22-02-2018 03:04:05','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 1, 'Brno', 
         TO_TIMESTAMP('25-03-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'));

INSERT INTO Event (Name, Description, CreationTime, CreationPlace, CreatedBy,
            EventLocation, EventTime) VALUES('Hodinka poezie',
            'Prednasat bude nas oblubeny autor zahranicnnej poezie pan Istvan
            Habodaj. Nenechajte si tuto prilezitost uniknut.', 
         TO_TIMESTAMP('22-03-2018 12:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 6, 'Komarno', 
         TO_TIMESTAMP('22-04-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'));     

INSERT INTO Event (Name, Description, CreationTime, CreationPlace, CreatedBy,
            EventLocation, EventTime) VALUES('SQL SELECTs',
            'Do you have a database and want to know your data? Learn how to
            use SELEECT statements in SQL scripts.', 
         TO_TIMESTAMP('27-03-2018 01:02:03','DD-MM-YYYY HH24:MI:SS'),
         'Nove Zamky', 2, 'Brno', 
         TO_TIMESTAMP('25-03-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'));

-------------------------------------------------------------------------------
------------------------------      USREVENT    -------------------------------
-------------------------------------------------------------------------------              
INSERT INTO UsrEvent (Usr, Event) VALUES(1, 6);
INSERT INTO UsrEvent (Usr, Event) VALUES(2, 6);
INSERT INTO UsrEvent (Usr, Event) VALUES(6, 6);
INSERT INTO UsrEvent (Usr, Event) VALUES(7, 6);
INSERT INTO UsrEvent (Usr, Event) VALUES(1, 7);
INSERT INTO UsrEvent (Usr, Event) VALUES(4, 7);
INSERT INTO UsrEvent (Usr, Event) VALUES(6, 7);
INSERT INTO UsrEvent (Usr, Event) VALUES(8, 7);
INSERT INTO UsrEvent (Usr, Event) VALUES(9, 7);
INSERT INTO UsrEvent (Usr, Event) VALUES(2, 8);
INSERT INTO UsrEvent (Usr, Event) VALUES(7, 8);
INSERT INTO UsrEvent (Usr, Event) VALUES(8, 8);
INSERT INTO UsrEvent (Usr, Event) VALUES(10, 8);   

-------------------------------------------------------------------------------
---------------------------------     POST   ----------------------------------
-------------------------------------------------------------------------------              
INSERT INTO Post (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Contents) VALUES('Republican Party', 'Briefy brief', 
         TO_TIMESTAMP('25-03-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'),
         'Hurbanovo', 1, 't would be great for the Republican Party of
         Nevada, and it''s unity if good guy Danny Tarkanian would run for
         Congress and Dean Heller');   
         
INSERT INTO Post (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Contents) VALUES('Trade CANADA', NULL, 
            TO_TIMESTAMP('25-03-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'),
         'Sturovo', 2, 'We do have a Trade Deficit with Canada, as we do 
         with almost all countries (some of them massive). P.M. Just in 
         Trudeau of Canada.');   

INSERT INTO Post (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Contents) VALUES('blind eye', NULL, 
            TO_TIMESTAMP('27-12-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'),
         'Vilnius', 2, 'We cannot keep a blind eye to the rampant unfair 
         trade practices against our Country!');   
   
INSERT INTO Post (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Contents) VALUES('fundamentals', NULL, 
            TO_TIMESTAMP('27-03-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'),
         'Krakow', 2, 'Perhaps at no time in history have the business 
         fundamentals of U.S. companies been better than they are today!');
            
INSERT INTO Post (Name, Description, CreationTime, CreationPlace, CreatedBy,
            Contents) VALUES('Economy', NULL, 
            TO_TIMESTAMP('19-03-2018 21:00:00','DD-MM-YYYY HH24:MI:SS'),
         'Sala', 6, 'The Economy is raging, at an all time high, and is set
         to get even better. Jobs and wages up. Vote for Rick Saccone 
            and keep it going!');   

-------------------------------------------------------------------------------
------------------------------      USRPOST     -------------------------------
-------------------------------------------------------------------------------     
INSERT INTO UsrPost (Usr, Post) VALUES(1, 9);
INSERT INTO UsrPost (Usr, Post) VALUES(2, 9);
INSERT INTO UsrPost (Usr, Post) VALUES(7, 12);

-------------------------------------------------------------------------------
------------------------------      KEYWORD     -------------------------------
-------------------------------------------------------------------------------
INSERT INTO Keyword (Keyword) VALUES('republican');
INSERT INTO Keyword (Keyword) VALUES('party');
INSERT INTO Keyword (Keyword) VALUES('economy');
INSERT INTO Keyword (Keyword) VALUES('fundamentals');
INSERT INTO Keyword (Keyword) VALUES('trade');
INSERT INTO Keyword (Keyword) VALUES('canada');
INSERT INTO Keyword (Keyword) VALUES('blind');
INSERT INTO Keyword (Keyword) VALUES('eye');
INSERT INTO Keyword (Keyword) VALUES('country');
INSERT INTO Keyword (Keyword) VALUES('tarkanian');
INSERT INTO Keyword (Keyword) VALUES('jobs');
INSERT INTO Keyword (Keyword) VALUES('putin');
INSERT INTO Keyword (Keyword) VALUES('crazed');
INSERT INTO Keyword (Keyword) VALUES('news');
INSERT INTO Keyword (Keyword) VALUES('excoriat');
INSERT INTO Keyword (Keyword) VALUES('crimes');
INSERT INTO Keyword (Keyword) VALUES('practices');
INSERT INTO Keyword (Keyword) VALUES('vote');

-------------------------------------------------------------------------------
------------------------------    POSTKEYWORD   -------------------------------
-------------------------------------------------------------------------------
INSERT INTO PostKeyword (Post, Keyword) VALUES(9,1);
INSERT INTO PostKeyword (Post, Keyword) VALUES(9,2);
INSERT INTO PostKeyword (Post, Keyword) VALUES(9,3);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,2);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,3);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,4);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,5);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,6);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,7);
INSERT INTO PostKeyword (Post, Keyword) VALUES(11,3);
INSERT INTO PostKeyword (Post, Keyword) VALUES(11,5);
INSERT INTO PostKeyword (Post, Keyword) VALUES(11,6);
INSERT INTO PostKeyword (Post, Keyword) VALUES(12,5);
INSERT INTO PostKeyword (Post, Keyword) VALUES(13,10);
INSERT INTO PostKeyword (Post, Keyword) VALUES(9,11);
INSERT INTO PostKeyword (Post, Keyword) VALUES(9,13);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,16);
INSERT INTO PostKeyword (Post, Keyword) VALUES(10,17);
INSERT INTO PostKeyword (Post, Keyword) VALUES(11,14);
INSERT INTO PostKeyword (Post, Keyword) VALUES(11,15);

-------------------------------------------------------------------------------
---------------------------------    PHOTO   ----------------------------------
-------------------------------------------------------------------------------
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('Vunderful', 'most cutting thing you can say is
            "whos this clown?" because it implies theyre a) a clown b) not even
            one of the better-known clowns', 
            TO_TIMESTAMP('22-11-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 1, 'Brno', 
         TO_TIMESTAMP('11-11-2017 17:07:22','DD-MM-YYYY HH24:MI:SS'), 1);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('Horses', 'How much for the horse tornado? Sir,
            thats a carousel. I must have it.', 
            TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 2, 'Brno', 
         TO_TIMESTAMP('11-12-2017 17:07:22','DD-MM-YYYY HH24:MI:SS'), 1);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('Click', '4 months ago i quietly left 57 dvds of
          click at my parents house and theyve still never noticed or 
             mentioned it', 
            TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
            'Vitkovice', 1, 'Ostrava', 
         TO_TIMESTAMP('11-12-2017 17:07:22','DD-MM-YYYY HH24:MI:SS'), 2);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('banana', 'Sorry I yelled "killin it" when your
            mom was eating that banana.', 
            TO_TIMESTAMP('01-01-2018 07:12:11','DD-MM-YYYY HH24:MI:SS'),
         'Bratislava', 3, 'Kuty', 
         TO_TIMESTAMP('01-01-2018 07:12:11','DD-MM-YYYY HH24:MI:SS'), 3);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('ChinesePizza', 'Damn girl are you a pizza at a 
            Chinese buffet because Im not feeling it right now but I see you
            over there doing you and I respect that.', 
            TO_TIMESTAMP('07-01-2018 18:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Ostrava', 4, 'Brno', 
            TO_TIMESTAMP('07-01-2018 19:07:22','DD-MM-YYYY HH24:MI:SS'), 3);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('Terrific', '"Ha ha ha! Terrific!" - Mitt Romney, 
            every time Jar Jar Binks appears on screen', 
            TO_TIMESTAMP('11-01-2018 11:05:22','DD-MM-YYYY HH24:MI:SS'),
         'Kromeriz', 5, 'Komarno', 
         TO_TIMESTAMP('16-01-2018 17:07:22','DD-MM-YYYY HH24:MI:SS'), 2);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('rapbattle', 'hi, grandma? can u come pick me up 
            from my rap battle? its over. no, i lost. he saw u drop me off did 
            a pretty devastating rhyme about it', 
            TO_TIMESTAMP('22-01-2018 18:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Praha', 6, 'Praha', 
         TO_TIMESTAMP('23-01-2018 17:07:22','DD-MM-YYYY HH24:MI:SS'), 4);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('show', '[after watching 1 episode of a show where 
          i paid attention roughly 10% of the time] This show sucks I dont under
         stand whats happening at all', 
            TO_TIMESTAMP('02-02-2018 19:24:12','DD-MM-YYYY HH24:MI:SS'),
         'Dulovce', 7, 'Dulovce', 
         TO_TIMESTAMP('02-02-2018 20:07:22','DD-MM-YYYY HH24:MI:SS'), 2);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('married', 'Until I got married I didnt even know 
            it was possible to chew bubblegum arrogantly.', 
            TO_TIMESTAMP('13-02-2018 18:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Brno', 7, 'Brno', 
         TO_TIMESTAMP('15-02-2018 17:07:22','DD-MM-YYYY HH24:MI:SS'), 2);
            
INSERT INTO Photo (Name, Description, CreationTime, CreationPlace, CreatedBy,
            CapturePlace, CaptureDate, BelongsTo)
            VALUES('Moon', NULL, 
            TO_TIMESTAMP('18-02-2018 11:07:22','DD-MM-YYYY HH24:MI:SS'),
         'Hurbanovo', 2, 'Brno', 
         TO_TIMESTAMP('21-02-2018 17:07:22','DD-MM-YYYY HH24:MI:SS'), 2);

-------------------------------------------------------------------------------
------------------------------      USRPHOTO    -------------------------------
-------------------------------------------------------------------------------
INSERT INTO UsrPhoto(Usr, Photo) VALUES(1, 15);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(2, 14);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(3, 17);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(3, 18);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(4, 15);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(7, 16);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(5, 20);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(3, 21);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(3, 22);
INSERT INTO UsrPhoto(Usr, Photo) VALUES(10, 22);

-------------------------------------------------------------------------------
-----------------------      MARK ALBUM TITLE PHOTO    ------------------------
-------------------------------------------------------------------------------
UPDATE Album
SET TitlePhoto = 14
WHERE AlbumID = 1;

UPDATE Album
SET TitlePhoto = 16
WHERE AlbumID = 2;

UPDATE Album
SET TitlePhoto = 18
WHERE AlbumID = 3;

UPDATE Album
SET TitlePhoto = 20
WHERE AlbumID = 4;

-------------------------------------------------------------------------------
------------------------------     COMMENTARY   -------------------------------
-------------------------------------------------------------------------------
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('I called President Putin of Russia to congratulate him on h
               is election victory (in past, Obama called him also). The F
               ake News Media is crazed because they wanted me to excoriat
               e him.', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS')
               , 'Nove Zamky', 1, 1);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('Special Council is told to find crimes, whether a crime e
               xists or not. I was opposed to the selection of Mueller to 
               be Special Council. I am still opposed to it. ', 
               TO_TIMESTAMP('27-12-2017 20:11:13','DD-MM-YYYY HH24:MI:SS'),
               NULL, 2, 2);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('There was no probable cause for believing that there was an
               y crime, collusion or otherwise, or obstruction of justice!
               ', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Komarno', 3, 2);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('Department of Justice should have urged the Supreme Court t
               o at least hear the Drivers License case on illegal immigra
               nts in Arizona.', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Budapest', 3, 3);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('AUSTIN BOMBING SUSPECT IS DEAD. Great job by law enforcemen
               t and all concerned!', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Bratislava', 3, 4);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('Our Nation was founded by farmers. Our independence was won
               by farmers. And our continent was tamed by farmers. Our far
               mers always lead the way', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               NULL, 5, 1);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('The Democrats do not want to help DACA. Would be so easy to
               make a deal!', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Kuty', 6, 7);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('A total WITCH HUNT with massive conflicts of interest!', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Breclav', 6, 8);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('Wow, watch Comey lie under oath to Senator G when asked "ha
               ve you ever been an anonymous source...or known someone els
               e to be an anonymous source...?"', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Pardubice', 7, 8);
               
INSERT INTO Commentary (Contents, SentOn, Place, Sender, ContainedIn) 
         VALUES('U.S. Consumer Confidence Hits 14-Year High', 
               TO_TIMESTAMP('24-12-2017 18:07:22','DD-MM-YYYY HH24:MI:SS'),
               'Vienna', 7, 9);
