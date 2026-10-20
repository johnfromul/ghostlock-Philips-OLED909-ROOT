# Opravený Philips OLED909 profil

Oprava je připravena jako **analysis-only integrace** do projektu na commitu
`1812ef35214f51ab7b8c37e935a62b074c66c141`. Nejde o ověření aktivní podpory TV.

## Opravy

- Úplný kernel release `5.15.180-android14-11-00042-g766efc243647-dirty`;
  nedochází ke kolizi s krátkým TCL identifikátorem.
- `.analysis_only=1`, konkrétní důvod blokace a explicitně neověřené route enumy.
- Báze `_text=0xffffffc008000000` a všech 32 symbolových offsetů přepočtených
  od této báze, konzistentně s projektovým převodem fyzických adres.
- Doplněné BTF layouty podpůrných objektů, mm_owner a page.slab_cache.
- Explicitní velikosti a další údaje: celý registry struct je inicializovaný.
- Nepřekrývající se lokální sloty podpůrných objektů; nejde o důkaz správné
  velikosti/reclaim geometrie skutečné alokace.
- Fyzické báze jsou označené jako statická firmware inference, nikoli měření.
- Opravený koncový oddělovač `),` i nepřesná tvrzení v komentářích.

## Neověřené hodnoty

Tyto hodnoty jsou explicitně 0 = neznámé:

- direct_map_end;
- kernelsnitch_identity_start a kernelsnitch_identity_end;
- mm_slab_order;
- slub_min_partial a slub_cpu_partial.

Projektové makro `_RSO` může pro nulovou položku stále zobrazit/použít
historickou výchozí hodnotu, pokud by někdo obešel výběrové kontroly.
Tyto fallbacky **nejsou validací Philips parametrů**. Opravená integrace
blokuje aktivaci přes analysis_only i neověřené route hodnoty. Samotné
vynulování analysis_only v profilu nestačí k přijetí profilu.

CFI callback kompatibilita, runtime RAM rozsah, allocator geometrie a
Philips cesty nadále vyžadují samostatné ověření. Tyto informace nelze
nahradit kopírováním parametrů TCL. Profil neměnit na aktivní na základě
tohoto metadata exportu.

## Použití

Samotný `philips_909_tpm231/offsets.h` vyžaduje doprovodnou integraci:
nové enum hodnoty, registraci v known_offsets a společný guard. Nepřidávejte
pouze hlavičku do staré verze registru.

Na čistém checkoutu uvedeného upstream commitu:

```console
git apply --check philips_oled909_analysis_only.patch
git apply philips_oled909_analysis_only.patch
make philips-profile-guard-test
```

Patch přidává novou hlavičku a host-only test, upravuje registr,
zobrazení neověřeného profilu a odmítnutí neověřených cest. Ostatní
zařízení zachovávají své původní hodnoty. Pokud již Philips soubor
v checkoutu existuje, patch pro přidání nového souboru se na něj přímo
neaplikuje; změny je třeba sloučit se stávající verzí.

## Ověření provedené zde

- `git apply --check` prošel proti uvedenému upstream checkoutu.
- Prošlo 9 offline testů: syntaxe C registru všech tří zařízení a přidaného
  host testu přes pycparser, jednoznačný release, všechny symbolové adresy,
  odstranění 14 nesprávných BTF fallbacků, neznámá geometrie, blokující
  výraz přímo z AST C helperu, použití guardu před publikací profilu a
  nepřekrývání lokálních slotů.
- Všechna pole kernel_offsets jsou explicitně inicializovaná.
- Nativní C test ani celý Android build zde nebyly kompilovány/spuštěny;
  host C kompilátor v pracovním prostředí není k dispozici.
- Nebyly provedeny žádné operace na TV.

`philips_oled909_corrected_profile.json` obsahuje všechny hodnoty a hash
opravované hlavičky. `philips909_profile_before.h` uchovává původní vstup.
