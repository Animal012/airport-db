PGDMP                      |            aeroflot    16.2    16.2 K    V           0    0    ENCODING    ENCODING        SET client_encoding = 'UTF8';
                      false            W           0    0 
   STDSTRINGS 
   STDSTRINGS     (   SET standard_conforming_strings = 'on';
                      false            X           0    0 
   SEARCHPATH 
   SEARCHPATH     8   SELECT pg_catalog.set_config('search_path', '', false);
                      false            Y           1262    16876    aeroflot    DATABASE     j   CREATE DATABASE aeroflot WITH TEMPLATE = template0 ENCODING = 'UTF8' LOCALE_PROVIDER = libc LOCALE = 'C';
    DROP DATABASE aeroflot;
                postgres    false            �            1255    17104 -   get_flights_by_destination(character varying)    FUNCTION       CREATE FUNCTION public.get_flights_by_destination(destination_name character varying) RETURNS TABLE(flight_date date, gate character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
    cur CURSOR FOR
        SELECT f.date, f.gate
        FROM flight f
        JOIN destination d ON f.destination_id = d.destination_id
        WHERE d.city = destination_name;
BEGIN
    OPEN cur;
    LOOP
        FETCH cur INTO flight_date, gate;
        EXIT WHEN NOT FOUND;
        RETURN NEXT;
    END LOOP;
    CLOSE cur;
END;
$$;
 U   DROP FUNCTION public.get_flights_by_destination(destination_name character varying);
       public          postgres    false            �            1255    17105 !   get_flights_in_period(date, date)    FUNCTION     c  CREATE FUNCTION public.get_flights_in_period(start_date date, end_date date) RETURNS TABLE(flight_number integer, flight_date date, aircraft_model character varying, destination_city character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
    FOR flight_number, flight_date, aircraft_model, destination_city IN
        SELECT f.number, f.date, a.model, d.city
        FROM flight f
        JOIN aircraft a ON f.aircraft_id = a.aircraft_id
        JOIN destination d ON f.destination_id = d.destination_id
        WHERE f.date BETWEEN start_date AND end_date
    LOOP
        RETURN NEXT;
    END LOOP;
END;
$$;
 L   DROP FUNCTION public.get_flights_in_period(start_date date, end_date date);
       public          postgres    false            �            1255    17109    update_flight_dates_to_2025()    FUNCTION     �  CREATE FUNCTION public.update_flight_dates_to_2025() RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
    flight_rec RECORD;
BEGIN
    FOR flight_rec IN
        SELECT flight_id, date
        FROM flight
        WHERE EXTRACT(YEAR FROM date) = 2024
    LOOP
        UPDATE flight
        SET date = flight_rec.date + INTERVAL '1 year'
        WHERE flight_id = flight_rec.flight_id;
    END LOOP;
END;
$$;
 4   DROP FUNCTION public.update_flight_dates_to_2025();
       public          postgres    false            �            1259    17002    aircraft    TABLE     �   CREATE TABLE public.aircraft (
    aircraft_id integer NOT NULL,
    model character varying(100) NOT NULL,
    airline character varying(100) NOT NULL,
    entry_into_service date NOT NULL
);
    DROP TABLE public.aircraft;
       public         heap    postgres    false            Z           0    0    TABLE aircraft    ACL     �   GRANT ALL ON TABLE public.aircraft TO admin2;
GRANT ALL ON TABLE public.aircraft TO employee;
GRANT SELECT ON TABLE public.aircraft TO passenger;
          public          postgres    false    224            �            1259    17001    aircraft_aircraft_id_seq    SEQUENCE     �   CREATE SEQUENCE public.aircraft_aircraft_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 /   DROP SEQUENCE public.aircraft_aircraft_id_seq;
       public          postgres    false    224            [           0    0    aircraft_aircraft_id_seq    SEQUENCE OWNED BY     U   ALTER SEQUENCE public.aircraft_aircraft_id_seq OWNED BY public.aircraft.aircraft_id;
          public          postgres    false    223            �            1259    16981    check_in_counter    TABLE     }   CREATE TABLE public.check_in_counter (
    counter_id integer NOT NULL,
    counter_number character varying(10) NOT NULL
);
 $   DROP TABLE public.check_in_counter;
       public         heap    postgres    false            \           0    0    TABLE check_in_counter    ACL     �   GRANT ALL ON TABLE public.check_in_counter TO admin2;
GRANT ALL ON TABLE public.check_in_counter TO employee;
GRANT SELECT ON TABLE public.check_in_counter TO passenger;
          public          postgres    false    218            �            1259    16980    check_in_counter_counter_id_seq    SEQUENCE     �   CREATE SEQUENCE public.check_in_counter_counter_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 6   DROP SEQUENCE public.check_in_counter_counter_id_seq;
       public          postgres    false    218            ]           0    0    check_in_counter_counter_id_seq    SEQUENCE OWNED BY     c   ALTER SEQUENCE public.check_in_counter_counter_id_seq OWNED BY public.check_in_counter.counter_id;
          public          postgres    false    217            �            1259    16995    destination    TABLE     �   CREATE TABLE public.destination (
    destination_id integer NOT NULL,
    iata_code character varying(3) NOT NULL,
    city character varying(100) NOT NULL,
    country character varying(100) NOT NULL
);
    DROP TABLE public.destination;
       public         heap    postgres    false            ^           0    0    TABLE destination    ACL     �   GRANT ALL ON TABLE public.destination TO admin2;
GRANT ALL ON TABLE public.destination TO employee;
GRANT SELECT ON TABLE public.destination TO passenger;
          public          postgres    false    222            �            1259    16994    destination_destination_id_seq    SEQUENCE     �   CREATE SEQUENCE public.destination_destination_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 5   DROP SEQUENCE public.destination_destination_id_seq;
       public          postgres    false    222            _           0    0    destination_destination_id_seq    SEQUENCE OWNED BY     a   ALTER SEQUENCE public.destination_destination_id_seq OWNED BY public.destination.destination_id;
          public          postgres    false    221            �            1259    16988    employee    TABLE     �   CREATE TABLE public.employee (
    employee_id integer NOT NULL,
    full_name character varying(100) NOT NULL,
    birth_date date NOT NULL,
    "position" character varying(50) NOT NULL
);
    DROP TABLE public.employee;
       public         heap    postgres    false            `           0    0    TABLE employee    ACL     ^   GRANT ALL ON TABLE public.employee TO admin2;
GRANT ALL ON TABLE public.employee TO employee;
          public          postgres    false    220            �            1259    16987    employee_employee_id_seq    SEQUENCE     �   CREATE SEQUENCE public.employee_employee_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 /   DROP SEQUENCE public.employee_employee_id_seq;
       public          postgres    false    220            a           0    0    employee_employee_id_seq    SEQUENCE OWNED BY     U   ALTER SEQUENCE public.employee_employee_id_seq OWNED BY public.employee.employee_id;
          public          postgres    false    219            �            1259    17026    flight    TABLE     �   CREATE TABLE public.flight (
    flight_id integer NOT NULL,
    number integer NOT NULL,
    date date NOT NULL,
    gate character varying(50) NOT NULL,
    aircraft_id integer,
    destination_id integer,
    counter_id integer
);
    DROP TABLE public.flight;
       public         heap    postgres    false            b           0    0    TABLE flight    ACL     �   GRANT ALL ON TABLE public.flight TO admin2;
GRANT ALL ON TABLE public.flight TO employee;
GRANT SELECT ON TABLE public.flight TO passenger;
          public          postgres    false    228            �            1259    17047    flight_employee    TABLE     j   CREATE TABLE public.flight_employee (
    flight_id integer NOT NULL,
    employee_id integer NOT NULL
);
 #   DROP TABLE public.flight_employee;
       public         heap    postgres    false            c           0    0    TABLE flight_employee    ACL     l   GRANT ALL ON TABLE public.flight_employee TO admin2;
GRANT ALL ON TABLE public.flight_employee TO employee;
          public          postgres    false    229            �            1259    17025    flight_flight_id_seq    SEQUENCE     �   CREATE SEQUENCE public.flight_flight_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 +   DROP SEQUENCE public.flight_flight_id_seq;
       public          postgres    false    228            d           0    0    flight_flight_id_seq    SEQUENCE OWNED BY     M   ALTER SEQUENCE public.flight_flight_id_seq OWNED BY public.flight.flight_id;
          public          postgres    false    227            �            1259    16974 	   passenger    TABLE     �   CREATE TABLE public.passenger (
    passenger_id integer NOT NULL,
    full_name character varying(100) NOT NULL,
    birth_date date NOT NULL,
    passport character varying(20) NOT NULL,
    email character varying(100)
);
    DROP TABLE public.passenger;
       public         heap    postgres    false            e           0    0    TABLE passenger    ACL     �   GRANT ALL ON TABLE public.passenger TO admin2;
GRANT ALL ON TABLE public.passenger TO employee;
GRANT SELECT,INSERT,UPDATE ON TABLE public.passenger TO passenger;
          public          postgres    false    216            �            1259    16973    passenger_passenger_id_seq    SEQUENCE     �   CREATE SEQUENCE public.passenger_passenger_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 1   DROP SEQUENCE public.passenger_passenger_id_seq;
       public          postgres    false    216            f           0    0    passenger_passenger_id_seq    SEQUENCE OWNED BY     Y   ALTER SEQUENCE public.passenger_passenger_id_seq OWNED BY public.passenger.passenger_id;
          public          postgres    false    215            �            1259    17009    ticket    TABLE     �   CREATE TABLE public.ticket (
    ticket_id integer NOT NULL,
    date date NOT NULL,
    flight_id integer,
    luggage character varying(50),
    gate character varying(50),
    passenger_id integer
);
    DROP TABLE public.ticket;
       public         heap    postgres    false            g           0    0    TABLE ticket    ACL     �   GRANT ALL ON TABLE public.ticket TO admin2;
GRANT ALL ON TABLE public.ticket TO employee;
GRANT SELECT,INSERT,UPDATE ON TABLE public.ticket TO passenger;
          public          postgres    false    226            �            1259    17008    ticket_ticket_id_seq    SEQUENCE     �   CREATE SEQUENCE public.ticket_ticket_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;
 +   DROP SEQUENCE public.ticket_ticket_id_seq;
       public          postgres    false    226            h           0    0    ticket_ticket_id_seq    SEQUENCE OWNED BY     M   ALTER SEQUENCE public.ticket_ticket_id_seq OWNED BY public.ticket.ticket_id;
          public          postgres    false    225            i           0    0    SEQUENCE ticket_ticket_id_seq    ACL     I   GRANT SELECT,USAGE ON SEQUENCE public.ticket_ticket_id_seq TO passenger;
          public          postgres    false    225            �           2604    17005    aircraft aircraft_id    DEFAULT     |   ALTER TABLE ONLY public.aircraft ALTER COLUMN aircraft_id SET DEFAULT nextval('public.aircraft_aircraft_id_seq'::regclass);
 C   ALTER TABLE public.aircraft ALTER COLUMN aircraft_id DROP DEFAULT;
       public          postgres    false    223    224    224            �           2604    16984    check_in_counter counter_id    DEFAULT     �   ALTER TABLE ONLY public.check_in_counter ALTER COLUMN counter_id SET DEFAULT nextval('public.check_in_counter_counter_id_seq'::regclass);
 J   ALTER TABLE public.check_in_counter ALTER COLUMN counter_id DROP DEFAULT;
       public          postgres    false    218    217    218            �           2604    16998    destination destination_id    DEFAULT     �   ALTER TABLE ONLY public.destination ALTER COLUMN destination_id SET DEFAULT nextval('public.destination_destination_id_seq'::regclass);
 I   ALTER TABLE public.destination ALTER COLUMN destination_id DROP DEFAULT;
       public          postgres    false    222    221    222            �           2604    16991    employee employee_id    DEFAULT     |   ALTER TABLE ONLY public.employee ALTER COLUMN employee_id SET DEFAULT nextval('public.employee_employee_id_seq'::regclass);
 C   ALTER TABLE public.employee ALTER COLUMN employee_id DROP DEFAULT;
       public          postgres    false    220    219    220            �           2604    17029    flight flight_id    DEFAULT     t   ALTER TABLE ONLY public.flight ALTER COLUMN flight_id SET DEFAULT nextval('public.flight_flight_id_seq'::regclass);
 ?   ALTER TABLE public.flight ALTER COLUMN flight_id DROP DEFAULT;
       public          postgres    false    227    228    228            �           2604    16977    passenger passenger_id    DEFAULT     �   ALTER TABLE ONLY public.passenger ALTER COLUMN passenger_id SET DEFAULT nextval('public.passenger_passenger_id_seq'::regclass);
 E   ALTER TABLE public.passenger ALTER COLUMN passenger_id DROP DEFAULT;
       public          postgres    false    216    215    216            �           2604    17012    ticket ticket_id    DEFAULT     t   ALTER TABLE ONLY public.ticket ALTER COLUMN ticket_id SET DEFAULT nextval('public.ticket_ticket_id_seq'::regclass);
 ?   ALTER TABLE public.ticket ALTER COLUMN ticket_id DROP DEFAULT;
       public          postgres    false    226    225    226            N          0    17002    aircraft 
   TABLE DATA           S   COPY public.aircraft (aircraft_id, model, airline, entry_into_service) FROM stdin;
    public          postgres    false    224   d]       H          0    16981    check_in_counter 
   TABLE DATA           F   COPY public.check_in_counter (counter_id, counter_number) FROM stdin;
    public          postgres    false    218   ^       L          0    16995    destination 
   TABLE DATA           O   COPY public.destination (destination_id, iata_code, city, country) FROM stdin;
    public          postgres    false    222   Y^       J          0    16988    employee 
   TABLE DATA           R   COPY public.employee (employee_id, full_name, birth_date, "position") FROM stdin;
    public          postgres    false    220    _       R          0    17026    flight 
   TABLE DATA           h   COPY public.flight (flight_id, number, date, gate, aircraft_id, destination_id, counter_id) FROM stdin;
    public          postgres    false    228   ``       S          0    17047    flight_employee 
   TABLE DATA           A   COPY public.flight_employee (flight_id, employee_id) FROM stdin;
    public          postgres    false    229   �`       F          0    16974 	   passenger 
   TABLE DATA           Y   COPY public.passenger (passenger_id, full_name, birth_date, passport, email) FROM stdin;
    public          postgres    false    216   �`       P          0    17009    ticket 
   TABLE DATA           Y   COPY public.ticket (ticket_id, date, flight_id, luggage, gate, passenger_id) FROM stdin;
    public          postgres    false    226   �b       j           0    0    aircraft_aircraft_id_seq    SEQUENCE SET     F   SELECT pg_catalog.setval('public.aircraft_aircraft_id_seq', 7, true);
          public          postgres    false    223            k           0    0    check_in_counter_counter_id_seq    SEQUENCE SET     M   SELECT pg_catalog.setval('public.check_in_counter_counter_id_seq', 8, true);
          public          postgres    false    217            l           0    0    destination_destination_id_seq    SEQUENCE SET     L   SELECT pg_catalog.setval('public.destination_destination_id_seq', 7, true);
          public          postgres    false    221            m           0    0    employee_employee_id_seq    SEQUENCE SET     F   SELECT pg_catalog.setval('public.employee_employee_id_seq', 7, true);
          public          postgres    false    219            n           0    0    flight_flight_id_seq    SEQUENCE SET     C   SELECT pg_catalog.setval('public.flight_flight_id_seq', 14, true);
          public          postgres    false    227            o           0    0    passenger_passenger_id_seq    SEQUENCE SET     H   SELECT pg_catalog.setval('public.passenger_passenger_id_seq', 7, true);
          public          postgres    false    215            p           0    0    ticket_ticket_id_seq    SEQUENCE SET     C   SELECT pg_catalog.setval('public.ticket_ticket_id_seq', 11, true);
          public          postgres    false    225            �           2606    17007    aircraft aircraft_pkey 
   CONSTRAINT     ]   ALTER TABLE ONLY public.aircraft
    ADD CONSTRAINT aircraft_pkey PRIMARY KEY (aircraft_id);
 @   ALTER TABLE ONLY public.aircraft DROP CONSTRAINT aircraft_pkey;
       public            postgres    false    224            �           2606    16986 &   check_in_counter check_in_counter_pkey 
   CONSTRAINT     l   ALTER TABLE ONLY public.check_in_counter
    ADD CONSTRAINT check_in_counter_pkey PRIMARY KEY (counter_id);
 P   ALTER TABLE ONLY public.check_in_counter DROP CONSTRAINT check_in_counter_pkey;
       public            postgres    false    218            �           2606    17000    destination destination_pkey 
   CONSTRAINT     f   ALTER TABLE ONLY public.destination
    ADD CONSTRAINT destination_pkey PRIMARY KEY (destination_id);
 F   ALTER TABLE ONLY public.destination DROP CONSTRAINT destination_pkey;
       public            postgres    false    222            �           2606    16993    employee employee_pkey 
   CONSTRAINT     ]   ALTER TABLE ONLY public.employee
    ADD CONSTRAINT employee_pkey PRIMARY KEY (employee_id);
 @   ALTER TABLE ONLY public.employee DROP CONSTRAINT employee_pkey;
       public            postgres    false    220            �           2606    17051 $   flight_employee flight_employee_pkey 
   CONSTRAINT     v   ALTER TABLE ONLY public.flight_employee
    ADD CONSTRAINT flight_employee_pkey PRIMARY KEY (flight_id, employee_id);
 N   ALTER TABLE ONLY public.flight_employee DROP CONSTRAINT flight_employee_pkey;
       public            postgres    false    229    229            �           2606    17031    flight flight_pkey 
   CONSTRAINT     W   ALTER TABLE ONLY public.flight
    ADD CONSTRAINT flight_pkey PRIMARY KEY (flight_id);
 <   ALTER TABLE ONLY public.flight DROP CONSTRAINT flight_pkey;
       public            postgres    false    228            �           2606    16979    passenger passenger_pkey 
   CONSTRAINT     `   ALTER TABLE ONLY public.passenger
    ADD CONSTRAINT passenger_pkey PRIMARY KEY (passenger_id);
 B   ALTER TABLE ONLY public.passenger DROP CONSTRAINT passenger_pkey;
       public            postgres    false    216            �           2606    17014    ticket ticket_pkey 
   CONSTRAINT     W   ALTER TABLE ONLY public.ticket
    ADD CONSTRAINT ticket_pkey PRIMARY KEY (ticket_id);
 <   ALTER TABLE ONLY public.ticket DROP CONSTRAINT ticket_pkey;
       public            postgres    false    226            �           2606    17099    ticket fk_ticket_flight    FK CONSTRAINT     �   ALTER TABLE ONLY public.ticket
    ADD CONSTRAINT fk_ticket_flight FOREIGN KEY (flight_id) REFERENCES public.flight(flight_id);
 A   ALTER TABLE ONLY public.ticket DROP CONSTRAINT fk_ticket_flight;
       public          postgres    false    228    226    3500            �           2606    17032    flight flight_aircraft_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.flight
    ADD CONSTRAINT flight_aircraft_id_fkey FOREIGN KEY (aircraft_id) REFERENCES public.aircraft(aircraft_id);
 H   ALTER TABLE ONLY public.flight DROP CONSTRAINT flight_aircraft_id_fkey;
       public          postgres    false    3496    228    224            �           2606    17042    flight flight_counter_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.flight
    ADD CONSTRAINT flight_counter_id_fkey FOREIGN KEY (counter_id) REFERENCES public.check_in_counter(counter_id);
 G   ALTER TABLE ONLY public.flight DROP CONSTRAINT flight_counter_id_fkey;
       public          postgres    false    218    3490    228            �           2606    17037 !   flight flight_destination_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.flight
    ADD CONSTRAINT flight_destination_id_fkey FOREIGN KEY (destination_id) REFERENCES public.destination(destination_id);
 K   ALTER TABLE ONLY public.flight DROP CONSTRAINT flight_destination_id_fkey;
       public          postgres    false    222    228    3494            �           2606    17057 0   flight_employee flight_employee_employee_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.flight_employee
    ADD CONSTRAINT flight_employee_employee_id_fkey FOREIGN KEY (employee_id) REFERENCES public.employee(employee_id);
 Z   ALTER TABLE ONLY public.flight_employee DROP CONSTRAINT flight_employee_employee_id_fkey;
       public          postgres    false    3492    220    229            �           2606    17052 .   flight_employee flight_employee_flight_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.flight_employee
    ADD CONSTRAINT flight_employee_flight_id_fkey FOREIGN KEY (flight_id) REFERENCES public.flight(flight_id);
 X   ALTER TABLE ONLY public.flight_employee DROP CONSTRAINT flight_employee_flight_id_fkey;
       public          postgres    false    3500    228    229            �           2606    17062    ticket ticket_passenger_id_fkey    FK CONSTRAINT     �   ALTER TABLE ONLY public.ticket
    ADD CONSTRAINT ticket_passenger_id_fkey FOREIGN KEY (passenger_id) REFERENCES public.passenger(passenger_id);
 I   ALTER TABLE ONLY public.ticket DROP CONSTRAINT ticket_passenger_id_fkey;
       public          postgres    false    3488    216    226            N   �   x�U���0�����@MۉGL�{����Eeɀ��a���������1��=
u��9}Bj�!!$F(MR;�Em��+R��pXd.cz��YL�w�~V��,�e�*:"S���8d�!S���q�)��0V�N�9�����7� ��I��u _.�AL      H   9   x����0���

@��b迎�g5�Z~M�nə��b*.���R���U�p ֤�      L   �   x�U��
�PE�y#8��:� 8~LmA����p+�R���~����p�&$7�'�H���pA�kh�Y�%/�y�JԟLe�>B|q��p�=^ً2f�6��=���
{B? G��T�O��Hd�Ģ->��Uj9]�ɲ4^���"����3o��$�M8b�����:5�Æ��E�hQPJ}~Ϗ/      J   0  x�m�kN�0�ۧ���Mڻp����H<DU-70}����fo��PRd�����)���A�
��Xɍ��J�C�U��Xl�n�G�*�*7ʬ�=c�
jJ�:Ux�[�<QY�K�`0�����K�u�*�<P;�)�B�E-֤��'�Q��E0:v��Zg,1}%�m��K�j�Wx�Y꣟ǐ��G�%�]`�8�����њ�	��&�
sn�������?I�xD�hD�I3%%C]����4�u�����h)��mW�GOU�fƆ�X���I(��Σ�����L�š����3��'�	:�      R   J   x�=ɱ�0�Xߋ�����:�8`�.X
�%(U-�Ix���1hhz6�Y� ��ʘJ�(\���x�0!      S   "   x�34�4�24�4�\���D�r��qqq V�      F   �  x�]�KN�0�דS���$v�]��@hB��JK��x,��B$PA�J�=��FLLZ(ǯ����G >��q�o��w�.��+m���퇋W4�W�����t5[���U̸`��H�(>4{i밗6r������/Wg8�1��[��y���O/h���	}�e�3A�D���q*H�~B�V>2N�L2�uU.^ۊ�����H�W"��+&|�cB�����Az��6�Vy��UI/qN���d�c�`j�_TGOt��6VU iH?�1ױ��e�I�u��+ZE��F24y��X����������֚x�ȷ��"�x�|TTsb�ϳ~9�I�,[����ec�{�7vB�UkuN�<0!A�AM �N�����Y��GaK��y��6A�fӧ:(-�X��s^3 76�l`:�C����s=QA      P   j   x���4202�50�54�44��~a�=�]�raÅ���&f��\�pUF��Ɯ.6U�b�]vo���idl�i�eh�0�l(�&�f@K�b���� ?X6n     