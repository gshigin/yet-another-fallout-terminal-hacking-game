// Copyright 2024-2025 Gleb Shigin. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// yafth
#include <yafth/core/engine_detail/symbol_manipulator.h>
#include <yafth/core/engine_detail/terminal_buffer.h>

namespace yafth::core::engine_detail::symbol_manipulator::_internal
{
auto generate_words(std::size_t length, std::size_t count, rng_fref gen_next) noexcept -> std::array<std::string, 20>
{
    constexpr size_t word_array_size = 100;
    std::array<std::string, word_array_size> word_array{};
    if (length == 4)
    {
        word_array = {"MINX", "ANNI", "PANG", "PIRO", "OKRO", "OAKS", "TOWS", "VOAR", "SLAY", "TUNY", "MUZZ", "MOLA", "NOLO", "NIGH", "DIKE", "OILY", "CARY",
                      "LAWS", "JEWS", "SAWS", "TACK", "HAIL", "SAKI", "SELL", "SEAN", "BEND", "RUDD", "FYRD", "OGRE", "COVE", "COSS", "HUNS", "LUNA", "XYLA",
                      "KAHA", "WADI", "ADDY", "JASY", "JOLL", "HOGG", "OONS", "KOBI", "JIBE", "SMEE", "SPUE", "SPUD", "IBAD", "DIAU", "FEAT", "LETO", "FONO",
                      "FERE", "NURL", "OKRA", "OKAY", "SPAY", "SLUP", "GEUM", "TEEM", "PREX", "PODS", "YOWT", "LOBI", "JIBS", "PAIS", "URIC", "TRIP", "ORAE",
                      "BOCE", "MOLS", "BOLK", "BOBA", "LEDA", "WEST", "MECH", "TACO", "KIHO", "KURI", "DURN", "CLAN", "CLAG", "STAG", "SOCO", "YOUL", "ROUN",
                      "POXY", "PYLA", "KOLI", "SELT", "BEET", "OPES", "MINS", "WICE", "MANE", "LARD", "LIFT", "SICH", "SLEE", "ALBS", "YRBK"};
    }
    if (length == 5)
    {
        word_array = {"ZANZE", "LAYNE", "MATTE", "MAILE", "PRILL", "PROMS", "PIGMY", "TIGRE", "LITRE", "FLARE", "GLEDE", "SLOKE", "BLOND", "AHIND", "SHUNT",
                      "RIANT", "RIFER", "REDES", "HERES", "HEUAU", "BELAM", "BETAS", "ELIAS", "REIFS", "NAIFS", "DAWKS", "DOBOS", "OOZOA", "FOLIA", "CILIA",
                      "CUBIT", "PURIM", "MINIM", "HINCH", "HIKER", "HALED", "OPTED", "MUTED", "PUTTO", "PURSY", "TORSI", "TOADS", "TUSKS", "GUANS", "LULLS",
                      "WOOLS", "SORUS", "HOBOS", "HOSEL", "NOWEL", "ROVED", "RAXED", "HAREM", "EASER", "LAXER", "GAMER", "LAMBA", "CUMBU", "CHIMU", "COILS",
                      "CARES", "BARIT", "TRAIT", "GNAST", "ORANT", "FRAIL", "GRANT", "OMANI", "GLALI", "GLOGG", "GEOTY", "WEBBY", "RELAY", "RAKAN", "DAYAK",
                      "KANAP", "KETAL", "TETEL", "TAMER", "JAPER", "TOPEE", "ESSEE", "VASES", "JISMS", "BILOS", "INKOS", "USKOK", "KOKOS", "KORAH", "MYNAH",
                      "MYCOL", "MYALL", "SKULL", "FAULX", "FAURD", "LLUDD", "PLUFF", "PLOTX", "PHONO", "PROPS"};
    }
    if (length == 6)
    {
        word_array = {"INTOMB", "MITOME", "MICKLE", "PICULS", "RECURS", "REWORK", "REFONT", "REBEAT", "BEHEAD", "BESCAB", "FESTAE", "LUSTRE", "CUITLE",
                      "DOTTLE", "POTPIE", "POTHER", "CIPHER", "FILTER", "FALCES", "NAWIES", "TASSES", "CASSIA", "CUSSES", "PULSUS", "OUTSUM", "ANISUM",
                      "ANATUM", "TECTUM", "VECTIS", "KELTIC", "CELTIS", "COMTES", "COULIS", "COMMIS", "CADMIA", "LAMMIE", "LAIOSE", "PAROLE", "PAGODA",
                      "PAGING", "SKYING", "BESING", "DECINE", "DEARIE", "HEINIE", "GLINSE", "KLIPPE", "KLEPHT", "FLEWIT", "FLORID", "ELOHIM", "EPODIC",
                      "EPODES", "ECHOES", "ECTOPY", "ECTYPE", "EATAGE", "PEWAGE", "DYKAGE", "CORAGE", "LOCATE", "LICHTS", "LISSES", "LYSSAS", "PASSUS",
                      "PAISAN", "KHIVAN", "BHUTAN", "PHYTIN", "AZOTIN", "CROTON", "PROTYL", "PROSOS", "GRASPS", "TRAITS", "BRAZES", "BRASSY", "BRUSHY",
                      "BOUSER", "ROUSTS", "MOUNTY", "MOGUEY", "WOGIET", "LOPPET", "POCKET", "PONGEE", "BONGOS", "BOOTHS", "QUOTES", "GUILES", "GRAPES",
                      "PHAGES", "CHAFFS", "CHALTA", "EXACTA", "EXISTS", "SUINTS", "SPIERS", "SIZERS", "VOTERS"};
    }
    if (length == 7)
    {
        word_array = {"KINESIS", "KINLESS", "BINDERS", "BUCKERS", "BUCKISH", "TURKISM", "AORTISM", "TOURISM", "MAURIST", "MARXISM", "DARKISH", "FASTISH",
                      "PETTISH", "SERFISH", "SERBIAN", "SENECAN", "SELECTS", "DIRECTS", "DEPICTS", "INDICTS", "INDENTS", "ASSENTS", "ARSENYL", "ACRINYL",
                      "ARRIVAL", "VARICAL", "VARIOLA", "CARIOCA", "CARANDA", "VARANUS", "PARADOS", "ABRADES", "PIRATES", "LYSATES", "REDATES", "REWARMS",
                      "REMARRY", "LEMANRY", "BEFANCY", "BESAUCE", "BELADLE", "VEXABLE", "WOOABLE", "WOMANLY", "WOMMALA", "WANGALA", "LANTANA", "MANTAPA",
                      "MANDATS", "RANDALL", "JAWFALL", "HEWHALL", "MECHAEL", "MACHAON", "MARSOON", "BALLOON", "GALLEIN", "DARREIN", "CARRYON", "CAMARON",
                      "CANADOL", "CANDIOT", "CONCION", "CONTEMN", "CONVERT", "CONGEST", "CONTUSE", "NONBASE", "DIABASE", "DISPACE", "DIAPASM", "DISCASE",
                      "DISCOUS", "DIGITUS", "DOCIOUS", "ROYNOUS", "AGYNOUS", "AENEOUS", "ADMETUS", "AMMITES", "ASEITAS", "EDEITIS", "EUECTIC", "CZECHIC",
                      "CHECHIA", "RHACHIS", "ROTCHES", "VOUCHES", "CINCHES", "CANCHAS", "PANCHAX", "PARCHED", "POTCHER", "MOUCHES", "POUNCES", "PLUSSES",
                      "PLUNGER", "PLINKER", "PLINIAN", "CLIVIAS"};
    }
    if (length == 8)
    {
        word_array = {"PYRECTIC", "EPHECTIC", "EMPESTIC", "EMPHATIC", "AMNIATIC", "AMNIOTIC", "ANEROTIC", "ANALOGIC", "ANALOGIA", "ANATOLIC",
                      "ANARCHIC", "AGARICIC", "ATARAXIC", "ATARAXIA", "SPARAXIS", "STYRAXES", "SOCRATES", "FERRATES", "PERCALES", "PERCOIDS",
                      "MERMAIDS", "PERTAINS", "SETTAINE", "GETTABLE", "OUTFABLE", "CITEABLE", "LIKEABLE", "LOVEABLY", "SORTABLY", "SETTABLE",
                      "ABATABLE", "EVADABLE", "VOIDABLE", "PLICABLE", "PROVABLE", "PROVINCE", "PROVIRUS", "PROVOKES", "PROPONER", "PROMPTER",
                      "PRONATOR", "PROFANER", "PROFILED", "PREMIXED", "PREVENED", "PRETENDS", "PROXENUS", "PROCERES", "PROPERER", "CREPEIER",
                      "GREEDIER", "SHEENIER", "SHEETFED", "SHEATHER", "SKETCHER", "SCUTCHES", "SCUTTERS", "STUTTERS", "SHUTTERS", "SHOCKERS",
                      "WHOPPERS", "DROPPERS", "CROPPIES", "CROPLESS", "CURBLESS", "MORNLESS", "HOSTLESS", "LISTLESS", "SICKLESS", "SHIPLESS",
                      "SUPPRESS", "OUTDRESS", "HUNDREDS", "SUNDRESS", "RINDLESS", "PANGLESS", "PANCREAS", "PANACEAN", "PATACOON", "NATALOIN",
                      "CATALOGS", "CATALASE", "CALABASH", "SAGANASH", "PAGANISM", "TURANISM", "BALANISM", "BALANDRA", "CALENDRY", "CULINARY",
                      "SEMINARY", "GEMINATE", "LEVIGATE", "LIMITATE", "LITHIATE", "MITIGATE", "VIGILATE", "VELICATE", "DELICACY", "DELICTUM"};
    }
    if (length == 9)
    {
        word_array = {"POTHUNTER", "MANHUNTER", "MISHANTER", "DISSENTER", "DISSEIZEE", "DISSHIVER", "DISSEIZOR", "DISSECTOR", "TRISECTOR", "PRORECTOR",
                      "PROTECTED", "PRODUCTID", "PREDICTED", "PREEMPTED", "PREEMPLOY", "PREEMBODY", "DISEMBODY", "DISEMBALM", "DISEMBARK", "DISIMPARK",
                      "DISIMMURE", "DISFIGURE", "MISFIGURE", "SUBFIGURE", "CONFIGURE", "CONFITURE", "COMFITURE", "COTORTURE", "DEPORTURE", "DEBENTURE",
                      "CALENTURE", "CALENDULA", "LAVANDULA", "SAVANILLA", "BATATILLA", "PARATITLA", "PARAMIMIA", "PARAKILYA", "PARABULIA", "PARABASIS",
                      "PARABASAL", "PARABANIC", "PARAGENIC", "GASOGENIC", "MISOGYNIC", "MONOGENIC", "NONYLENIC", "NONPOETIC", "NONEXOTIC", "NONATOMIC",
                      "TRIATOMIC", "TRICROTIC", "ANACROTIC", "ANALEPTIC", "ANELASTIC", "OOBLASTIC", "ORGIASTIC", "HYGIASTIC", "HYBRISTIC", "TOURISTIC",
                      "TORYISTIC", "DEREISTIC", "DUALISTIC", "CLADISTIC", "ITACISTIC", "IDOLISTIC", "PHOTISTIC", "ZIONISTIC", "PIETISTIC", "PIERROTIC",
                      "PLEURITIC", "PLEURITIS", "PYLORITIS", "MYLONITES", "MIGONITIS", "LIMONITES", "YEMENITES", "REIGNITES", "REINCITES", "COINCIDES",
                      "PRINCIPES", "PRINCOXES", "PRIMROSES", "PRIAPUSES", "PRIAPULID", "PRIAPULUS", "PREADULTS", "NONADULTS", "NONGUILTS", "BONGOISTS",
                      "MONODISTS", "MONOLITHS", "AEROLITHS", "AEROBIOUS", "STROBILUS", "STROBILAE", "STRIGINAE", "STRIVINGS", "STUDDINGS", "SLEDDINGS"};
    }
    if (length == 10)
    {
        word_array = {
            "DIFFUSEDLY", "DISPOSEDLY", "SUPPOSEDLY", "SUPPOSABLE", "PROPOSABLE", "COMPOSABLE", "COMMONABLE", "MOTIONABLE", "RATIONABLY", "NOTIONALLY",
            "NOTIONABLE", "NONNOTABLE", "NONSALABLE", "NONVOLUBLE", "CONSOLABLE", "COAGULABLE", "COAGULATES", "COAGULANTS", "POSTULANTS", "PORTOLANOS",
            "PORTULACAS", "POSTULATES", "POSTULATOR", "POSTFACTOR", "CONTRACTOR", "CONTRACTEE", "CONTRAMURE", "NONERASURE", "NONCLOSURE", "NONCULTURE",
            "NONRUPTURE", "NONVESTURE", "NONWESTERN", "NONMASTERY", "NONFACTORY", "PREFACTORY", "PREHISTORY", "PREDIATORY", "PREDICTORY", "PREDICTION",
            "PROAUCTION", "PRELECTION", "DEFLECTION", "DEFAMATION", "DETONATION", "DEFEDATION", "DEFINITION", "DEFINITISE", "DEFINITELY", "DIMINUTELY",
            "GEMINATELY", "FEMININELY", "FEMININITY", "SEMINUDITY", "DEMINUDITY", "FEMINILITY", "NOMINALITY", "NOMINALISM", "NOMINATING", "LAMINATING",
            "PUMICATING", "FUGITATING", "MILITATING", "MILITARILY", "DILATORILY", "DILATINGLY", "DOUBTINGLY", "WOBBLINGLY", "WARBLINGLY", "WAVERINGLY",
            "WATCHINGLY", "FETCHINGLY", "TEACHINGLY", "TRAPPINGLY", "GRASPINGLY", "GLANCINGLY", "GLOSSINGLY", "PLEASINGLY", "CREAKINGLY", "SPEAKINGLY",
            "SPONGINGLY", "STINGINGLY", "STABBINGLY", "SLASHINGLY", "STARRINGLY", "SNORTINGLY", "SNUFFINGLY", "SKULKINGLY", "SOUNDINGLY", "SLINKINGLY",
            "THINKINGLY", "TRIMMINGLY", "TRIPPINGLY", "WHIPPINGLY", "WHISKINGLY", "WRITHINGLY", "SOOTHINGLY", "SCOWLINGLY", "SCOLDINGLY", "SKIDDINGLY"};
    }
    if (length == 11)
    {
        word_array = {"SCRIMPINGLY", "SHRINKINGLY", "THRILLINGLY", "THRONGINGLY", "SHRUGGINGLY", "SHRIEKINGLY", "SPRINGINGLY", "STRINGENTLY", "STRINGENDOS",
                      "STRANGERDOM", "STRANGERING", "SQUANDERING", "UNWANDERING", "UNLINGERING", "ENDANGERING", "UNBANTERING", "UNHAMPERING", "UNHALTERING",
                      "INFILTERING", "ENFETTERING", "EMBITTERING", "UNTITTERING", "UNDIFFERING", "UNSUFFERING", "UNSOLDERING", "RESOLDERING", "RESILVERING",
                      "REGISTERING", "PLAISTERING", "PILASTERING", "SPLATTERING", "SPLATHERING", "INGATHERING", "UNWITHERING", "UNWONDERING", "UNCONDUCING",
                      "UNCONVENING", "UNCOMBINING", "UNCONFIDING", "UNCONSUMING", "UNPRESUMING", "UNPREPARING", "REPREPARING", "REPROPOSING", "REPROACHING",
                      "APPROACHING", "ENCROACHING", "ENWREATHING", "INBREATHING", "INTRENCHING", "RETRENCHING", "BEDRENCHING", "REBRANCHING", "RESEARCHING",
                      "RESEARCHERS", "RESEARCHFUL", "RESEARCHIST", "PETRARCHIST", "PETRARCHIAN", "PETRARCHIZE", "TETRARCHATE", "TETRARCHIES", "HIERARCHIES",
                      "HIERARCHIAL", "HIERARCHIZE", "HIERARCHIST", "HEPTARCHIST", "HEPTARCHIES", "PENTARCHIES", "POLYARCHIES", "POLYORCHISM", "POLYCROTISM",
                      "POLYGLOTISM", "POLYGLOTTED", "POLYGLOTTER", "POLYGLOTTAL", "POLYODONTAL", "PERIODONTAL", "PERIORBITAL", "PERIPROCTAL", "PERITROCHAL",
                      "PERITRICHAN", "PERITRICHIC", "PERIGRAPHIC", "MOGIGRAPHIC", "MONOGRAPHIC", "ALLOGRAPHIC", "ALLOMORPHIC", "CECOMORPHIC", "MESOMORPHIC",
                      "HEMIMORPHIC", "MEROMORPHIC", "ENDOMORPHIC", "THEOMORPHIC", "ZYGOMORPHIC", "HALOMORPHIC", "HOMOMORPHIC", "MONOMORPHIC", "HYLOMORPHIC",
                      "CYNOMORPHIC"};
    }
    if (length == 12)
    {
        word_array = {"BULLETMAKING", "BASKETMAKING", "CARPETMAKING", "BARRELMAKING", "TASSELMAKING", "TESSELLATING", "TESSELLATION", "INSTELLATION",
                      "CULTELLATION", "CRENELLATION", "CORRELLATION", "CORRADIATION", "TRIRADIATION", "NONRADIATION", "NONCAUSATION", "NONVALUATION",
                      "REEVALUATION", "ABEVACUATION", "PREVALUATION", "PREDAMNATION", "PRESALVATION", "PRESERVATION", "PRESELECTION", "PREDETENTION",
                      "PREREJECTION", "PREDECEPTION", "PREDETECTION", "PREOBJECTION", "PREOBJECTIVE", "PREEFFECTIVE", "NONEFFECTIVE", "NONDIRECTIVE",
                      "NONDIRECTION", "MISDIRECTION", "MISAFFECTION", "DISAFFECTION", "NONAFFECTION", "NONDEFECTION", "NONDEPLETION", "NONDEDUCTION",
                      "PREDEDUCTION", "PREDEPLETION", "PREDEPLETING", "PREDECREEING", "PREDECLINING", "PRERECEIVING", "PREDESERVING", "PREBESETTING",
                      "PREREMITTING", "PREADMITTING", "PREALLOTTING", "PREALLOTMENT", "NONALLOTMENT", "NONALINEMENT", "NONATONEMENT", "NONAMENDMENT",
                      "PROAMENDMENT", "PROAMUSEMENT", "PROAGREEMENT", "PREAGREEMENT", "DISAGREEMENT", "DISGORGEMENT", "DISBURSEMENT", "DISGUISEMENT",
                      "DISGRACEMENT", "DISENACTMENT", "DISPLACEMENT", "DISPROVEMENT", "DISPERSEMENT", "DISPIRITMENT", "DISHERITMENT", "DISHEVELMENT",
                      "DISSEVERMENT", "DISINTERMENT", "DISINTERRING", "MISINFERRING", "MISINTERRING", "MISREFERRING", "PREREFERRING", "PREREVENGING",
                      "PREREVERSING", "NONREVERSING", "NONRECEIVING", "NONDECEIVING", "NONDEFECTING", "NONAFFECTING", "NONADDICTING", "NONADDICTIVE",
                      "NONASSISTIVE", "NONRESISTIVE", "NONRADIATIVE", "NONMEDIATIVE", "NONMEDIATION", "NONSATIATION", "NONVARIATION", "NONPURGATION",
                      "NONPROBATION", "NONPLUSATION", "NONEDUCATION", "NONEXECUTION"};
    }

    std::array<std::string, 20> words;
    std::size_t i = 0;
    do
    {
        std::string next_word = word_array[gen_next() % word_array_size];
        if (std::find(words.begin(), words.end(), next_word) == words.end())
        {
            words[i++] = next_word;
        }
    } while (i < count);

    return words;
}
void generate_term_chars(terminal_buffer &terminal, rng_fref gen_next) noexcept
{
    static constexpr std::array<char, 24> placeholders = {'.', ',', '!', '?', '/', '*', '+', '\'', ':', ';', '-', '_',
                                                          '%', '$', '|', '@', '{', '}', '[', ']',  '(', ')', '<', '>'};

    for (size_t i = 0; i < terminal.size(); ++i)
    {
        terminal.set(i, placeholders[gen_next() % placeholders.size()]);
    }
}
auto place_words(terminal_buffer &terminal, const std::array<std::string, 20> &words, std::size_t length, std::size_t count,
                 rng_fref gen_next) noexcept -> std::array<uint16_t, 20>
{
    std::array<uint16_t, 20> offsets{};

    const std::size_t space_per_word = terminal.size() / count;
    const std::size_t possible_start = space_per_word - length;
    for (std::size_t id = 0; id < count; ++id)
    {
        offsets[id] = id * space_per_word + gen_next() % possible_start;
        terminal.replace_with_string(offsets[id], words[id]);
    }

    return offsets;
}
} // namespace yafth::core::engine_detail::symbol_manipulator::_internal