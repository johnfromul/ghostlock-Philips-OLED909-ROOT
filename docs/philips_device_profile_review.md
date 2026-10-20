# Kontrola Philips OLED909 device profilu

**Výsledek: zatím nepřidávat jako aktivní podporované zařízení.** Symbolová a
BTF metadata jsou konzistentní, ale integrace do současného projektu má níže
uvedené blokující chyby. Soubor nebyl změněn a žádný exploit ani operace na TV
nebyly spuštěny.

Kontrolovaný soubor:
`C:/Users/admin/AppData/Local/hermes/cache/scratch/ghostlock-analysis/src/devices/philips_909_tpm231/offsets.h`

SHA-256: `041f21992005effd4e14cc00f917a3b7fa2916947253ea79c61be280fa39a95b`.
Referenční projekt: commit `1812ef35214f51ab7b8c37e935a62b074c66c141`.
Kontrola neznamená ověření případných pozdějších změn vzdálené větve main.

## Zjištění podle závažnosti

### P1 — Báze `_stext` není kompatibilní s projektovými převody (řádky 13–16)

Samotné sčítání deklarovaných off_* s `_stext=0xffffffc008010000` dává
správné link-time virtuální adresy. Projekt však používá kimage_text_base
jako referenci fyzického začátku Image `_text`.

`p0_data_alias()` počítá fyzickou adresu jako
`kernel_phys_load + (image_addr - KIMAGE_TEXT_BASE)`. Zadané
kernel_phys_load=0x26000000 je staticky odvozená fyzická adresa `_text`.
Pro init_task proto tento profil dává 0x2898c4c0, zatímco stejná statická
konfigurace a symbolový offset od `_text` dávají 0x2899c4c0: chyba -0x10000.

Navíc perf větev v main.c zarovnává anchor na 2 MiB a vyžaduje vzdálenost
od kimage_text_base dělitelnou 2 MiB. Báze končící 0x10000 touto kontrolou
neprojde pro žádný takto zarovnaný anchor.

Pro nezměněný projekt použít konzistentní dvojici `_text` a offsetů od
`_text`, kterou obsahuje dříve vytvořený philips_oled909_tcl_symbol_fields.inc.
Alternativou by byla širší změna a ověření všech adresních převodů; pouhá
změna názvu komentáře nepostačuje. Statická fyzická báze stále není runtime důkaz.

Zdrojové opory:
[util.c](https://github.com/Philiphall6/ghostlock-tcl-T653T01-ROOT/blob/1812ef35214f51ab7b8c37e935a62b074c66c141/src/core/util.c#L832),
[main.c](https://github.com/Philiphall6/ghostlock-tcl-T653T01-ROOT/blob/1812ef35214f51ab7b8c37e935a62b074c66c141/src/core/main.c#L1717).

### P1 — Neověřený profil nemá `analysis_only` (řádek 10)

Vynechané pole uint8_t je v C nulové. `select_offsets()` pak profil po
úspěšném výběru přijme jako aktivní. Současně vynechané stack_overlay_route
a reclaim_route jsou nula, což vybírá reference SEQPACKET/Sabrina, nikoli
ověřenou Philips cestu. Žádná Philips runtime validace takové cesty zde není.

Pro přidání metadat zachovat explicitní `.analysis_only=1` a popis důvodu
v analysis_blocker. Není podloženo přepínat profil na aktivní pouze na
základě správných adres. Stejný mechanismus používá projekt pro TCL profil.

### P1 — Chybí fyzická a mapovací geometrie (řádek 13)

Profil udává kernel_phys_load, ale nikoli phys_offset. V select_offsets()
se při zadané bázi 0x26000000 a nulovém phys_offset neprovede aktualizace
p0_phys_offset, takže zůstane target.h default **0x80000000**.
To neodpovídá fyzickému základu 0x20000000 uvedenému ve zkoumaných
Philips firmware memory_info datech.

Vynechaný direct_map_end navíc převezme 0xffffff9000000000 a chybějí
Philips údaje pro identity span. Ne všechny ostatní fallbacky jsou nutně
chybné: např. PAGE_OFFSET nebo VMEMMAP_START mohou odpovídat konfiguraci.
Nelze však z toho považovat celou geometrii za ověřenou. Je nutné ji
explicitně doložit pro tento build, včetně slab order a SLUB parametrů;
samotné sizeof(mm_struct) slab order nedokazuje.

### P1 — Chybějící layouty přebírají nesprávné fallbacky (řádky 97–121)

Zkontrolované explicitní cred/task/fops/rt_mutex hodnoty sedí. Vynechaná
pole ale nejsou automaticky získána z BTF. runtime_struct_offsets.h použije
výchozí konstanty a 14 z kontrolovaných výchozích hodnot se liší od Philips BTF:

| Pole | Efektivní fallback | Philips BTF |
| --- | --- | --- |
| mm_owner | 0x410 | 0x348 |
| struct_slab_cache (v tomto jádře page.slab_cache) | 0x08 | 0x18 |
| task_security_size | 0x40 | 0x18 |
| user_struct_size | 0x80 | 0xa8 |
| ucounts_size | 0x100 | 0x90 |
| user_ns_size | 0x100 | 0x290 |
| user_ns_gid_map | 0x40 | 0x48 |
| user_ns_projid_map | 0x80 | 0x90 |
| user_ns_ns | 0xd8 | 0xf0 |
| user_ns_ucounts | 0xf0 | 0x208 |
| user_ns_ucount_max | 0xf8 | 0x210 |
| user_ns_ucount_max_count | 1 | 14 |
| uid_gid_map_size | 0x40 | 0x48 |
| uid_gid_map_extent | 0x04 | 0x08 |

Některé makrodefinice nejsou v tomto checkoutu přímo používány, ale
user_namespace a podpůrné objekty mají skutečné čtenáře v sestavování
datových layoutů v util.c. Například mapy jsou vytvořeny na chybných
offsetech a namespace je nulován jen do délky 0x100 místo 0x290.
Po doplnění skutečných velikostí je nutné znovu ověřit i nepřekrývání
umístění podpůrných objektů; nepřebírat rozmístění pro menší fallbacky.

### P2 — Release klíč nematchuje Philips build a koliduje s TCL (řádek 10)

find_offsets_for_release() používá přesné strcmp, nikoli prefix. Soubor
uvádí `5.15.180-android14-11`, ale extrahované jádro má UTS release
`5.15.180-android14-11-00042-g766efc243647-dirty`.

Při této skutečné release se profil nevybere. Krátký klíč je navíc totožný
s existujícím TCL profilem: při takové release by rozhodovalo pořadí
registrace a po připojení za TCL by Philips profil byl nedosažitelný.
Pro tento build je třeba jednoznačný identifikátor odpovídající skutečnému
uname -r, případně explicitní kontrola další identity zařízení.

### P2 — Soubor nelze přímo vložit mezi položky `known_offsets[]` (řádek 136)

Soubor končí `)`, ale OFFSETS_ENTRY expanduje na `{ ... }` bez čárky.
Repozitář zahrnuje profilové soubory přímo do pole, za nimiž následuje
další položka nebo sentinel. Profil proto potřebuje na konci `),`.
Bez oddělovače vložení do pole neprojde kompilací.

Samotné vytvoření složky zařízení také nezajišťuje registraci:
src/devices/offsets.h musí profil explicitně zahrnout. V kontrolovaném
upstream checkoutu zahrnuje pouze Sabrina a TCL. Související registry a
výběrové testy nebyly součástí přiloženého souboru.

## Co bylo ověřeno jako správné

- 32 off_* hodnot souhlasí s Philips symboly / odvozenými položkami při
  výslovně deklarované bázi `_stext`.
- Celkem prošlo 99 kontrol explicitních číselných polí včetně zadaných
  task_struct, cred, rt_mutex_waiter, rt_mutex_base a file_operations polí.
- selinux_state.enforcing je bool na offsetu 0, velikost 1 bajt.
- ashmem_misc.fops je na +0x10; off_ashmem_misc_fops=0x29d8f50 při `_stext`.
- copy_splice_read není přítomen a 0 označuje nedostupnost, nikoli callback.
- kernel_phys_load=0x26000000 je konzistentní s dřívější statickou
  analýzou bootloaderu, ale nelze jej vydávat za runtime měření.

Komentář o ověření přes /proc/kallsyms je nepřesný: zde byly symboly
rekonstruovány z offline obrazu. Přes ADB čteny nebyly. Také
`Seccomp:0` znamená vypnutý seccomp; není důvodem nečitelnosti souboru.

CFI zůstává integračním bodem k ověření: reálné ashmem callbacky obsahují
adresy CFI trampoline, zatímco off_ashmem_* udávají těla funkcí. Správnost
symbolové tabulky sama nepotvrzuje zaměnitelnost těchto adres při volání
přes funkční ukazatel.

Strojový audit všech kontrol a fallbacků je v
`philips_device_profile_audit.json`. Projekt nebyl kompilován ani spuštěn.
Po odstranění nálezů je potřeba samostatný offline integrační test výběru
profilu, efektivních makrohodnot, odmítnutí neověřených cest a adresních
převodů. Teprve to může prokázat připravenost k přidání jako analysis-only profil.
