#pragma once

#include <yafth/core/engine_detail/terminal_buffer.h>

#include <array>
#include <string>

namespace yafth::core::engine_detail::symdol_manipulator
{
template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
auto generate_words(std::size_t length, std::size_t count, URBG &&g) noexcept -> std::array<std::string, 20>
{
    constexpr size_t word_array_size = 100;
    std::array<std::string, word_array_size> word_array{};
    if (length == 4)
    {
        word_array = {"THAT", "WITH", "FROM", "WERE", "THIS", "THEY", "HAVE", "SAID", "WHAT", "WHEN", "BEEN", "THEM", "INTO", "MORE", "ONLY", "WILL", "THEN",
                      "SOME", "TIME", "SUCH", "VERY", "OVER", "YOUR", "THAN", "WELL", "DOWN", "FACE", "UPON", "LIKE", "SAME", "KNOW", "WENT", "MADE", "LONG",
                      "CAME", "ROOM", "MUST", "EVEN", "EYES", "COME", "MOST", "LIFE", "AWAY", "HAND", "LEFT", "ALSO", "JUST", "ARMY", "BACK", "GOOD", "HEAD",
                      "MARY", "PART", "FELT", "HERE", "MUCH", "BONE", "TAKE", "MANY", "SKIN", "TOOK", "ONCE", "LOOK", "LAST", "BOTH", "GIVE", "SIDE", "FORM",
                      "MAKE", "DOOR", "KNEW", "TELL", "TOLD", "WHOM", "LOVE", "DAYS", "DEAR", "SEEN", "GAVE", "CASE", "DONE", "FREE", "SOON", "EACH", "WORK",
                      "LESS", "WIFE", "DOES", "MIND", "BODY", "OPEN", "WANT", "SENT", "HALF", "YEAR", "PAIN", "WORD", "HOME", "ANNA", "FIND"};
    }
    if (length == 5)
    {
        word_array = {"WHICH", "THERE", "THEIR", "WOULD", "COULD", "AFTER", "OTHER", "ABOUT", "THESE", "THOSE", "FIRST", "WHERE", "UNDER", "STILL", "BEING",
                      "AGAIN", "BEGAN", "GREAT", "ASKED", "WHILE", "COUNT", "WHOLE", "SHALL", "RIGHT", "PLACE", "STATE", "HOUSE", "EVERY", "HEARD", "YOUNG",
                      "NEVER", "THREE", "YEARS", "THINK", "ROUND", "FOUND", "BLOOD", "POWER", "MIGHT", "SMALL", "QUITE", "LARGE", "VOICE", "WORDS", "HANDS",
                      "CASES", "AMONG", "SONYA", "OFTEN", "TAKEN", "SMILE", "KNOWN", "ALONG", "ORDER", "WOMEN", "NIGHT", "STOOD", "JOINT", "CAUSE", "GOING",
                      "GIVEN", "WORLD", "CHIEF", "FRONT", "WHITE", "ALONE", "HORSE", "LATER", "DEATH", "LABOR", "MONEY", "UNTIL", "WOMAN", "NERVE", "SOUTH",
                      "WOUND", "THING", "LEAVE", "ABOVE", "ADDED", "PARTY", "PARTS", "TABLE", "BORIS", "ENEMY", "CRIED", "LIGHT", "UNION", "EARLY", "PETYA",
                      "BONES", "SINCE", "HEART", "FORMS", "SPEAK", "MEANS", "MOVED", "FORCE", "THIRD", "SHORT"};
    }
    if (length == 6)
    {
        word_array = {"PIERRE", "PRINCE", "BEFORE", "SHOULD", "ANDREW", "FRENCH", "LITTLE", "STATES", "PEOPLE", "ROSTOV", "LOOKED", "MOSCOW", "SEEMED",
                      "HAVING", "THOUGH", "ALWAYS", "TISSUE", "UNITED", "FATHER", "DURING", "TURNED", "MOMENT", "HOLMES", "CALLED", "BATTLE", "OTHERS",
                      "BECOME", "BEHIND", "COURSE", "RESULT", "PASSED", "MATTER", "ACTION", "TOWARD", "ALMOST", "THINGS", "TROOPS", "BECAME", "WITHIN",
                      "MOTHER", "TAKING", "NUMBER", "EITHER", "LETTER", "PUBLIC", "COMMON", "FRIEND", "SECOND", "CANNOT", "GLANDS", "ITSELF", "REALLY",
                      "TWENTY", "AROUND", "SAYING", "BETTER", "HORSES", "VASILI", "SYSTEM", "FORMED", "FELLOW", "LONGER", "MYSELF", "BEYOND", "GROWTH",
                      "ANYONE", "ORDERS", "TUMOUR", "ACROSS", "RATHER", "BESIDE", "COMING", "MAKING", "OPENED", "WANTED", "RAISED", "FAMILY", "WISHED",
                      "SEEING", "ANSWER", "EVENTS", "PERIOD", "GERMAN", "RUSSIA", "SPREAD", "MIDDLE", "REASON", "RETURN", "MERELY", "SILENT", "EFFECT",
                      "GIVING", "WINDOW", "WOUNDS", "PERSON", "DINNER", "DOCTOR", "PLACED", "TRYING", "STREET"};
    }
    if (length == 7)
    {
        word_array = {"NATASHA", "HIMSELF", "WITHOUT", "THOUGHT", "ANOTHER", "GENERAL", "THROUGH", "AGAINST", "BETWEEN", "NOTHING", "EMPEROR", "BECAUSE",
                      "DISEASE", "KUTUZOV", "USUALLY", "LOOKING", "ALREADY", "CHAPTER", "OFFICER", "RUSSIAN", "HISTORY", "DENISOV", "HOWEVER", "COUNTRY",
                      "BROUGHT", "PATIENT", "SEVERAL", "FEELING", "CERTAIN", "WHETHER", "HERSELF", "MORNING", "PRESENT", "REPLIED", "ENGLAND", "AMERICA",
                      "TISSUES", "PROJECT", "EXAMPLE", "CARRIED", "ENTERED", "SURFACE", "SITTING", "BRITISH", "EVENING", "SHOUTED", "VESSELS", "BECOMES",
                      "DRAWING", "FORWARD", "HUNDRED", "RESULTS", "WOUNDED", "SERVICE", "ANATOLE", "REACHED", "STRANGE", "PROCESS", "OPINION", "SOLDIER",
                      "AFFAIRS", "ENGLISH", "SLAVERY", "STOPPED", "PERHAPS", "TURNING", "APPLIED", "MUSCLES", "TALKING", "RAPIDLY", "HUSBAND", "ABSCESS",
                      "COMPANY", "NOTICED", "TUMOURS", "FEDERAL", "SUBJECT", "BELIEVE", "WAITING", "QUICKLY", "REMOVED", "ACCOUNT", "BROTHER", "FOREIGN",
                      "GLANCED", "FREEDOM", "COVERED", "SOCIETY", "CHANGES", "NEITHER", "SMILING", "ROSTOVS", "SOMEONE", "VILLAGE", "LESIONS", "SPECIAL",
                      "MEMBERS", "FINALLY", "FRIENDS", "VARIOUS"};
    }
    if (length == 8)
    {
        word_array = {"PRINCESS", "AMERICAN", "NICHOLAS", "NAPOLEON", "COUNTESS", "SUDDENLY", "CONGRESS", "POSITION", "SOLDIERS", "ANYTHING",
                      "QUESTION", "MOVEMENT", "POSSIBLE", "DOLOKHOV", "FOLLOWED", "BUSINESS", "OFFICERS", "NATIONAL", "RECEIVED", "TOGETHER",
                      "THOUSAND", "EVERYONE", "PRESSURE", "CHILDREN", "REMAINED", "REGIMENT", "MILITARY", "ANSWERED", "INTEREST", "STANDING",
                      "PRESENCE", "ANEURYSM", "REPEATED", "HAPPENED", "COLONIES", "AFFECTED", "APPEARED", "SOUTHERN", "TERRIBLE", "RETURNED",
                      "FEATURES", "CAMPAIGN", "SPEAKING", "DAUGHTER", "STRENGTH", "COLONIAL", "MEASURES", "SYMPTOMS", "SYPHILIS", "GANGRENE",
                      "REMARKED", "ALTHOUGH", "SWELLING", "ADJUTANT", "YOURSELF", "REMEMBER", "CLINICAL", "CONTRARY", "EMPLOYED", "LISTENED",
                      "MEMBRANE", "PAVLOVNA", "RUSSIANS", "VIRGINIA", "PROBABLY", "COMPLETE", "GOVERNOR", "CARRIAGE", "DECLARED", "CONSISTS",
                      "PLEASURE", "RELATION", "PREPARED", "ALPATYCH", "PROPERTY", "THINKING", "DISEASES", "INDUSTRY", "ATTENDED", "PRODUCED",
                      "ACTIVITY", "OBSERVED", "DRESSING", "PEASANTS", "EXPECTED", "THOUGHTS", "STRAIGHT", "MAJORITY", "OCCURRED", "ECONOMIC",
                      "COMMERCE", "ELECTION", "DISTANCE", "GENERALS", "HANDSOME", "OCCUPIED", "POLITICS", "SHOULDER", "BACTERIA", "INCREASE"};
    }
    if (length == 9)
    {
        word_array = {"SOMETHING", "TREATMENT", "SOMETIMES", "EVIDENTLY", "PRESIDENT", "INFECTION", "CONDITION", "NECESSARY", "COMMANDER", "CONTINUED",
                      "IMPORTANT", "DIFFERENT", "GUTENBERG", "POLITICAL", "FORMATION", "OPERATION", "FOLLOWING", "ATTENTION", "THEREFORE", "QUESTIONS",
                      "BOLKONSKI", "CHARACTER", "ACCORDING", "BAGRATION", "ALEXANDER", "DESCRIBED", "RELATIONS", "DIFFICULT", "SECONDARY", "DIRECTION",
                      "BEGINNING", "HAPPINESS", "INCREASED", "INFLUENCE", "EXPRESSED", "BOURIENNE", "DESTROYED", "EVERYBODY", "JEFFERSON", "ORGANISMS",
                      "SHOULDERS", "RESULTING", "PRISONERS", "CERTAINLY", "PRESENTED", "ARTICULAR", "EXCLAIMED", "INTERESTS", "MOVEMENTS", "DIAGNOSIS",
                      "TERRITORY", "SUFFERING", "VARIETIES", "CARTILAGE", "FRENCHMAN", "AUTHORITY", "DISCHARGE", "GENTLEMEN", "GENTLEMAN", "BEAUTIFUL",
                      "DEMOCRACY", "WHISPERED", "BONAPARTE", "DEMOCRATS", "THOUSANDS", "DANGEROUS", "ESSENTIAL", "APPOINTED", "GENERALLY", "ROOSEVELT",
                      "AMENDMENT", "GRADUALLY", "LISTENING", "INDICATED", "MALIGNANT", "SITUATION", "NECESSITY", "BRILLIANT", "INCLUDING", "AGITATION",
                      "CONVINCED", "SOVEREIGN", "COMPANION", "CONCERNED", "SURPRISED", "AMERICANS", "SPERANSKI", "BENNIGSEN", "OURSELVES", "PROMINENT",
                      "PARALYSIS", "SEPARATED", "HURRIEDLY", "AFFECTION", "COMPANIES", "SENSATION", "ABANDONED", "ADDRESSED", "CAREFULLY", "ARTHRITIS"};
    }
    if (length == 10)
    {
        word_array = {
            "GOVERNMENT", "EVERYTHING", "UNDERSTAND", "ESPECIALLY", "EXPRESSION", "THEMSELVES", "IMPOSSIBLE", "PETERSBURG", "CONDITIONS", "UNDERSTOOD",
            "FREQUENTLY", "REVOLUTION", "ASSOCIATED", "REPUBLICAN", "WASHINGTON", "CONSIDERED", "CONVENTION", "SYPHILITIC", "FRIGHTENED", "APPEARANCE",
            "HISTORIANS", "EXCELLENCY", "ROSTOPCHIN", "REMEMBERED", "INDIVIDUAL", "IMPORTANCE", "DIFFICULTY", "EXPERIENCE", "DMITRIEVNA", "SURROUNDED",
            "CONNECTION", "CONNECTIVE", "POPULATION", "INDUSTRIAL", "OPPOSITION", "TENDERNESS", "COMPLETELY", "PARTICULAR", "AFFECTIONS", "FOUNDATION",
            "RECOGNIZED", "PRINCIPLES", "APPROACHED", "CONCEPTION", "DEMOCRATIC", "INTRODUCED", "OPERATIONS", "THROUGHOUT", "REMARKABLE", "PERIOSTEUM",
            "SUFFICIENT", "ADDRESSING", "AMPUTATION", "AFTERWARDS", "ENTERPRISE", "PROPORTION", "SETTLEMENT", "ABSOLUTELY", "COMPROMISE", "CHARACTERS",
            "INCREASING", "APPARENTLY", "COMMERCIAL", "CONTAINING", "IMPRESSION", "OCCURRENCE", "ACCUSTOMED", "HISTORICAL", "INTERESTED", "PARLIAMENT",
            "COMMANDERS", "DETERMINED", "INDUSTRIES", "IRRITATION", "PROTECTION", "RECOGNISED", "COMMISSION", "EXCITEMENT", "INEVITABLE", "EVERYWHERE",
            "CONCLUSION", "ELECTRONIC", "LEADERSHIP", "LEUCOCYTES", "RESOLUTION", "SUCCESSFUL", "CALIFORNIA", "EPITHELIUM", "POSSESSION", "PREVIOUSLY",
            "DIPLOMATIC", "CONFIDENCE", "PROTECTIVE", "ULCERATION", "UNPLEASANT", "ASSISTANCE", "HYPERAEMIA", "OCCUPATION", "PERMISSION", "PROVISIONS"};
    }
    if (length == 11)
    {
        word_array = {"TUBERCULOUS", "IMMEDIATELY", "HAEMORRHAGE", "SUPPURATION", "REPUBLICANS", "MIKHAYLOVNA", "ESTABLISHED", "DEVELOPMENT", "DESTRUCTION",
                      "INTERRUPTED", "TEMPERATURE", "EXPERIENCED", "SURROUNDING", "SUPERFICIAL", "CIRCULATION", "CONTINUALLY", "POSSIBILITY", "ACCOMPANIED",
                      "GRANULATION", "INHABITANTS", "LEGISLATURE", "MISSISSIPPI", "APPLICATION", "INDEPENDENT", "INFORMATION", "EXAMINATION", "INTERESTING",
                      "APPROACHING", "IMMIGRATION", "OPPORTUNITY", "CONTRACTION", "LEGISLATION", "DISAPPEARED", "EXPLANATION", "AGRICULTURE", "AUTHORITIES",
                      "ENLARGEMENT", "CONSIDERING", "FEDERALISTS", "PROGRESSIVE", "TRANSFERRED", "CONNECTICUT", "DECLARATION", "IMAGINATION", "BOGUCHAROVO",
                      "EPITHELIOMA", "CONTINENTAL", "ASSOCIATION", "COMBINATION", "TERRITORIES", "DISTURBANCE", "REPRESENTED", "CICATRICIAL", "DISTRIBUTED",
                      "SENSIBILITY", "APPOINTMENT", "COMPRESSION", "OBSERVATION", "SIGNIFICANT", "DISLOCATION", "INNUMERABLE", "BATTLEFIELD", "DISPOSITION",
                      "OBLIGATIONS", "EXCEPTIONAL", "RESTORATION", "UNFORTUNATE", "ARRANGEMENT", "RECOGNIZING", "CONFEDERATE", "CORPORATION", "DISTINGUISH",
                      "GOVERNMENTS", "CONTROVERSY", "INQUIRINGLY", "PREPARATION", "RECOGNITION", "APPROPRIATE", "ATTENTIVELY", "EXCEEDINGLY", "IMPROVEMENT",
                      "INDIVIDUALS", "PRACTICALLY", "RESPONSIBLE", "APPEARANCES", "INDIFFERENT", "LEGISLATIVE", "NECESSARILY", "RECOMMENDED", "SUPPURATIVE",
                      "UNDOUBTEDLY", "CONFEDERACY", "CONNECTIONS", "UNNECESSARY", "COMPLICATED", "CONVENTIONS", "PHILIPPINES", "CONSEQUENCE", "CONSTITUTES",
                      "CONTRACTURE"};
    }
    if (length == 12)
    {
        word_array = {"CONSTITUTION", "ILLUSTRATION", "CONVERSATION", "PARTICULARLY", "CONSIDERABLE", "INDEPENDENCE", "MADEMOISELLE", "SUBCUTANEOUS",
                      "PENNSYLVANIA", "INFLAMMATION", "OCCASIONALLY", "SIGNIFICANCE", "INTERFERENCE", "TUBERCULOSIS", "LEGISLATURES", "SATISFACTION",
                      "PHILADELPHIA", "DEGENERATION", "DISTRIBUTION", "GRANULATIONS", "UNEXPECTEDLY", "ACQUAINTANCE", "HANDKERCHIEF", "DIFFICULTIES",
                      "INFLAMMATORY", "HEADQUARTERS", "INSTRUCTIONS", "PROCLAMATION", "CONSERVATIVE", "PREPARATIONS", "MANUFACTURES", "SURROUNDINGS",
                      "ORGANIZATION", "RESPECTFULLY", "SATISFACTORY", "CIVILIZATION", "INTELLECTUAL", "ACCOMPLISHED", "LEUCOCYTOSIS", "NEVERTHELESS",
                      "RATIFICATION", "REGENERATION", "OSSIFICATION", "PATHOLOGICAL", "DISPOSITIONS", "DISSATISFIED", "INSTITUTIONS", "COMBINATIONS",
                      "NEGOTIATIONS", "PRESIDENTIAL", "SUFFICIENTLY", "CONCENTRATED", "INTRODUCTION", "ARRANGEMENTS", "CONSEQUENTLY", "ASTONISHMENT",
                      "PROFESSIONAL", "IMPROVEMENTS", "CORPORATIONS", "DISPLACEMENT", "INDIFFERENCE", "SUBSEQUENTLY", "ACCUMULATION", "CONSTRUCTION",
                      "INTERVENTION", "SUCCESSFULLY", "FIBROMATOSIS", "INFILTRATION", "VERESHCHAGIN", "AFFECTIONATE", "APPLICATIONS", "DELIBERATELY",
                      "EMANCIPATION", "LYMPHANGITIS", "PASSIONATELY", "RECOLLECTION", "REPRESENTING", "RESTRICTIONS", "AGRICULTURAL", "ESTABLISHING",
                      "IRRESISTIBLE", "ANNOUNCEMENT", "CONGRATULATE", "DEMONSTRATED", "DIFFERENTIAL", "HENDRIKHOVNA", "REQUIREMENTS", "ACCIDENTALLY",
                      "MILORADOVICH", "COMMENCEMENT", "COMPENSATION", "DISTRIBUTING", "STRENGTHENED", "TRANQUILLITY", "ACCOMPANYING", "ADMINISTERED",
                      "ADVANTAGEOUS", "COLONIZATION", "CONSEQUENCES", "CONSIDERABLY"};
    }

    std::array<std::string, 20> words;
    std::size_t i = 0;
    do
    {
        std::string next_word = word_array[std::forward<URBG>(g)() % word_array_size];
        if (std::find(words.begin(), words.end(), next_word) == words.end())
        {
            words[i++] = next_word;
        }
    } while (i < count);

    return words;
}

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
void generate_term_chars(terminal_buffer &terminal, URBG &&g) noexcept
{
    static constexpr std::array<char, 24> placeholders = {'.', ',', '!', '?', '/', '*', '+', '\'', ':', ';', '-', '_',
                                                          '%', '$', '|', '@', '{', '}', '[', ']',  '(', ')', '<', '>'};

    for (size_t i = 0; i < terminal.size(); ++i)
    {
        terminal.set(i, placeholders[std::forward<URBG>(g)() % placeholders.size()]);
    }
}

template <class URBG>
    requires std::uniform_random_bit_generator<std::remove_reference_t<URBG>>
auto place_words(terminal_buffer &terminal, const std::array<std::string, 20> &words, std::size_t length, std::size_t count, URBG &&g) noexcept
    -> std::array<uint16_t, 20>
{
    std::array<uint16_t, 20> offsets{};

    const std::size_t space_per_word = terminal.size() / count;
    const std::size_t possible_start = space_per_word - length;
    for (std::size_t id = 0; id < count; ++id)
    {
        offsets[id] = id * space_per_word + std::forward<URBG>(g)() % possible_start;
        terminal.replace_with_string(offsets[id], words[id]);
    }

    return offsets;
}

} // namespace yafth::core::engine_detail::symdol_manipulator