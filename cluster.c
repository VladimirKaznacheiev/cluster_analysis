/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>
#include <errno.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

#define BUFFERSIZE 100
/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    c->capacity = cap;
    c->obj = malloc(cap*sizeof(struct obj_t));

}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->obj = NULL;
    c->capacity = 0;
    c->size = 0;

}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
   if (c->size == c->capacity) resize_cluster(c, c->capacity+CLUSTER_CHUNK);
   c->obj[c->size] = obj;
   c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    if (c2->size !=0 && c2->obj != NULL){
        for (int object_index = 0; object_index< c2->size; object_index++){
            append_cluster(c1, c2->obj[object_index]);
        }
    }
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);
    for (int index = idx; index<narr; index++){
        carr[index] = carr[index+1];
        
    }
    return narr-1;

}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float x_diff = o1->x - o2->x;
    float y_diff = o1->y - o2->y;
    return (float)sqrt((x_diff*x_diff) + (y_diff*y_diff));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float min_distance = INFINITY;
    for (int c1_index = 0; c1_index < c1->size; c1_index++){
        for (int c2_index = 0; c2_index < c2->size; c2_index++){
            float distance = obj_distance(&c1->obj[c1_index], &c2->obj[c2_index]);
            if (distance<min_distance) min_distance = distance;
        }
    }
    return min_distance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float min_distance = 0;
    float distance = 0;
    for (int index1 = 0; index1 < narr; index1++){
        for (int index2 = 0; index2 < narr; index2++){
            distance = cluster_distance(&carr[index1], &carr[index2]);
            if (distance > 0.0 && (min_distance == 0 || distance<min_distance)) {
                min_distance = distance;
                *c1 = index1;
                *c2 = index2;
                }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

void clear_memory(struct cluster_t *carr, int narr){
    for (int i = 0; i< narr; i++){
        clear_cluster(&carr[i]);
    }
    free(carr);
    carr = NULL;
}

float safe_atof(char *in_str){
    float out;
    char *end;
    errno = 0;

    out = strtof(in_str, &end);
    if (end == in_str)
        return -1.0;

    if ( errno == ERANGE)  
        return -1.0;

    return out;
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE* file;
    file = fopen(filename, "r");
    if (file == NULL){
        arr = NULL;
        return -1;
    }
    char buffer[BUFFERSIZE];
    fgets(buffer, BUFFERSIZE, file);
    char rows_number_str[BUFFERSIZE] = {};
    int number_index = 0;
    for (int char_index = 0; buffer[char_index] != '\n'; ++char_index){
        if(buffer[char_index] <= '9' && buffer[char_index] >= '0') {
            rows_number_str[number_index] = buffer[char_index];
            number_index++;
        }
    }

    int rows_number = atoi(rows_number_str);
    *arr = (struct cluster_t*)malloc(sizeof(struct cluster_t) * rows_number + 9);
    for (int row_index = 0; row_index < rows_number; row_index++){
        struct obj_t obj;
        struct cluster_t cluster;
        fgets(buffer, BUFFERSIZE, file);
        int word_index = 0;
        
        char *token = strtok(buffer, " ");
        
        while(token != NULL)
        {
            switch (word_index){
                case 0:
                    obj.id = (float)safe_atof(token);
                    break;
                case 1:;
                    float x = (float)safe_atof(token);
                    if (x<0 || x> 1000) {
                        clear_memory(*arr, rows_number);
                        fclose(file);
                        return -1;
                    }
                    obj.x = x;
                    break;
                case 2:;
                    float y = (float)safe_atof(token);
                    if (y<0 || y> 1000){
                        clear_memory(*arr, rows_number);
                        fclose(file);
                        return -1; 
                    }
                    obj.y = y;
                    break;
                default:
                    clear_memory(*arr, rows_number);
                    fclose(file);
                    return -1;
            }
            word_index++;
            token = strtok(NULL, " ");
        }
        if (word_index < 3){
            clear_memory(*arr, rows_number);
            fclose(file);
            return -1;
        }
        init_cluster(&cluster, CLUSTER_CHUNK);
        append_cluster(&cluster, obj);
        (*arr)[row_index] = cluster;

    }
    fclose(file);
    return rows_number;
}
 

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {   
        sort_cluster(&carr[i]);
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3){
        fprintf(stderr, "Wrong number of program arguments. ./cluster {filename} {number_of_clusters}\n");
        return EXIT_FAILURE;
    }
    char *filename = argv[1];
    int number_of_clusters = 1;
    if (argc == 3){
        for (int i = 0; i < (int)strlen(argv[2]); i++) {
            if(!(argv[2][i] <= '9' && argv[2][i] >= '0')){
                fprintf(stderr, "Wrong number of clusters! \n");
                return EXIT_FAILURE;
            }
        }
        number_of_clusters = atoi(argv[2]); 
    }
    struct cluster_t *clusters;
    int clusters_num = load_clusters(filename, &clusters);
    if (clusters_num == -1){
        fprintf(stderr, "Error in reading file\n");
        return EXIT_FAILURE;
    }
    while (clusters_num > number_of_clusters)
    {
        int c1;
        int c2;
        find_neighbours(clusters, clusters_num, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        clusters_num = remove_cluster(clusters, clusters_num, c2);
    }
    print_clusters(clusters, clusters_num);
    clear_memory(clusters, clusters_num);
    return EXIT_SUCCESS;
}