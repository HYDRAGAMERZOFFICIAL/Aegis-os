-- AegisOS cloud player accounts and isolated campaign saves.
-- Apply this migration to a dedicated Supabase project before deploying to Vercel.

create table if not exists public.player_profiles (
  id uuid primary key references auth.users(id) on delete cascade,
  username text not null unique,
  display_name text not null,
  created_at timestamptz not null default now(),
  constraint player_profiles_username_format check (username ~ '^[a-z0-9_-]{3,20}$'),
  constraint player_profiles_display_name_length check (char_length(display_name) between 2 and 32)
);

create table if not exists public.player_game_saves (
  user_id uuid primary key references auth.users(id) on delete cascade,
  save_data jsonb not null,
  updated_at timestamptz not null default now(),
  constraint player_game_saves_schema check (
    jsonb_typeof(save_data) = 'object'
    and save_data ? 'schemaVersion'
    and save_data ? 'game'
  )
);

create index if not exists player_game_saves_updated_at_idx
  on public.player_game_saves (updated_at desc);

create or replace function public.aegis_create_player_profile()
returns trigger
language plpgsql
security definer
set search_path = ''
as $$
declare
  profile_username text;
  profile_display_name text;
begin
  profile_username := lower(trim(coalesce(new.raw_user_meta_data ->> 'username', '')));
  profile_display_name := trim(coalesce(new.raw_user_meta_data ->> 'display_name', ''));

  if profile_username !~ '^[a-z0-9_-]{3,20}$' then
    raise exception 'A valid Aegis operator ID is required';
  end if;
  if char_length(profile_display_name) < 2 or char_length(profile_display_name) > 32 then
    raise exception 'A valid Aegis display name is required';
  end if;

  insert into public.player_profiles (id, username, display_name)
  values (new.id, profile_username, profile_display_name);

  return new;
end;
$$;

drop trigger if exists aegis_auth_user_created on auth.users;
create trigger aegis_auth_user_created
  after insert on auth.users
  for each row execute procedure public.aegis_create_player_profile();

alter table public.player_profiles enable row level security;
alter table public.player_game_saves enable row level security;

drop policy if exists "Players can read their own profile" on public.player_profiles;
create policy "Players can read their own profile"
  on public.player_profiles for select
  to authenticated
  using (auth.uid() = id);

drop policy if exists "Players can update their own profile" on public.player_profiles;
create policy "Players can update their own profile"
  on public.player_profiles for update
  to authenticated
  using (auth.uid() = id)
  with check (auth.uid() = id);

drop policy if exists "Players can read their own campaign save" on public.player_game_saves;
create policy "Players can read their own campaign save"
  on public.player_game_saves for select
  to authenticated
  using (auth.uid() = user_id);

drop policy if exists "Players can create their own campaign save" on public.player_game_saves;
create policy "Players can create their own campaign save"
  on public.player_game_saves for insert
  to authenticated
  with check (auth.uid() = user_id);

drop policy if exists "Players can update their own campaign save" on public.player_game_saves;
create policy "Players can update their own campaign save"
  on public.player_game_saves for update
  to authenticated
  using (auth.uid() = user_id)
  with check (auth.uid() = user_id);

revoke all on public.player_profiles from anon;
revoke all on public.player_game_saves from anon;
grant select, update on public.player_profiles to authenticated;
grant select, insert, update on public.player_game_saves to authenticated;
