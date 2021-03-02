<?php

/**
 * Menu callbacks.
 */

function handle_relative_links($links) {
    $http_host = $_SERVER['HTTP_HOST'];
    $retval = [];
    foreach ($links as $link) {
        #if (substr($link['href'], 0, 5) != 'http:') {
        if (substr($link['href'], 0, 1) == '/') {
            $link['href'] = '//'. $http_host . $link['href'];
        }
        $retval[] = $link;
    }
    return $retval;
}

//needed in the CORS context with the cache-busting strategy we've adopted
function set_response_headers() {
    drupal_add_http_header('Access-Control-Allow-Headers', 'Pragma');
    drupal_add_http_header('Access-Control-Allow-Headers', 'Cache-Control', TRUE);
    drupal_add_http_header('Access-Control-Allow-Headers', 'If-Modified-Since', TRUE);
}

/**
 * This version takes the gene "name" (or "uniquename"), looks up associated info from the db and delegates to the more fully specified version
 */
function gene_linkout_json($gene) {
    //find auxiliary info given the gene name
    //typically a polypeptide name (we stole this trick from the phylotree module)
        $sql = <<<SQL
          SELECT f.name AS gene_name, f.uniquename AS gene_uniquename, fp.value AS family_representative, o.genus AS genus, o.species AS species
          FROM chado.feature f
          JOIN chado.organism o ON o.organism_id = f.organism_id
          JOIN chado.cvterm cvt ON f.type_id = cvt.cvterm_id AND cvt.name = 'gene'
          LEFT JOIN chado.featureprop fp ON fp.feature_id = f.feature_id 
            AND fp.type_id = (SELECT cvterm_id FROM chado.cvterm
                              WHERE NAME = 'family representative')
          WHERE f.name = :gene OR f.uniquename = :gene
SQL;
        $args = array(':gene' => $gene);
        $result = chado_query($sql, $args);
        $fields = $result->fetchAssoc();
    if (isset($fields['gene_uniquename'])) {
        full_gene_linkout_json($fields['genus'], $fields['species'], $fields['gene_name'], $fields['gene_uniquename'], $fields['family_representative']);
    }
    else {
        gene_linkout_for_transcript_json($gene);
    }
}

/**
 * This version takes the transcript "name" (or "uniquename"), looks up associated gene from the db and delegates to the gene-based version
 */
function gene_linkout_for_transcript_json($transcript) {
    //find gene given the transcript name or uniquename
        $sql = <<<SQL
            SELECT g.uniquename AS gene_uniquename
            FROM chado.feature g
            JOIN chado.feature_relationship fr ON g.feature_id = fr.object_id
            JOIN chado.cvterm cvt ON fr.type_id = cvt.cvterm_id AND cvt.name = 'part_of'
            JOIN chado.feature t ON t.feature_id = fr.subject_id
            WHERE t.name = :transcript OR t.uniquename = :transcript
SQL;
        $args = array(':transcript' => $transcript);
        $result = chado_query($sql, $args);
        $fields = $result->fetchAssoc();
    if (isset($fields['gene_uniquename'])) {
        gene_linkout_json($fields['gene_uniquename']);
    }
    #return empty json appropriately
    else {
        set_response_headers();
        drupal_json_output(handle_relative_links([]));
        drupal_exit();
    }
}

function family_representative_linkout_json($genus, $species, $family_representative) {
    //find the gene by use of the "family representative" (ie label used for the phylonode, 
    //typically a polypeptide name (we stole this trick from the phylotree module)
        $sql = <<<SQL
          SELECT f.name AS gene_name, f.uniquename AS gene_uniquename
          FROM chado.featureprop fp, chado.feature f
          WHERE fp.type_id = (SELECT cvterm_id FROM chado.cvterm
                              WHERE NAME = 'family representative')
          AND fp.value = :family_representative
          AND fp.feature_id = f.feature_id
SQL;
        $args = array(':family_representative' => $family_representative);
        $result = chado_query($sql, $args);
        $fields = $result->fetchAssoc();
    full_gene_linkout_json($genus,$species,$fields['gene_name'],$fields['gene_uniquename'],$family_representative);
}

function full_gene_linkout_json($genus,$species,$gene_name,$gene_uniquename,$transcript_name) {
    $link =  [];
    //in what follows, the gene_name will typically not want the gensp. prefix, though this will be
    //readded where need be
    $gensp = substr($gene_name, 0, 6);
    //hack: we are moving towards not prefixing names with "yuck-lite". but we need to support older stuff where we still have it...
    if (preg_match('/^[a-z]{5}\./', $gensp)) {
        if (isset($gene_name)) {
            $gene_name = substr($gene_name, 6);
        }
        $transcript_name = substr($transcript_name, 6);
    }
    else {
        $gensp = "";
    }
    if (isset($gene_uniquename)) {
        $link[] = array(
            'href' => '/feature/'.$genus.'/'.$species.'/gene/'.$gene_uniquename,
            'text' => 'View LIS gene page for : ' . $gene_name,
        );
    }
    if (isset($gene_name)) {
        $link[] = array(
            'href' => '/gcv2/gene;lis=' . $gensp.$gene_name,
            'text' => 'Find similar genomic contexts at LIS for : ' . $gene_name,
        );
        $link[] = array(
            'href' => '/lis_context_viewer/search/lis/' . $gensp.$gene_name,
            'text' => 'Find similar genomic contexts at LIS for : ' . $gene_name . ' (GCV 1.0)',
        );
        $link[] = array(
            'href' => 'https://legumefederation.org/gcv/phytozome_10_2/search/lis/' . $gensp . $gene_name . '?sources=lfo&regexp=arath&neighbors=20&order=edit',
            'text' => 'Look for Arabidopsis thaliana microsyntenic region for ' . $gene_name,
        );
    }
    if (isset($gene_name)) {
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/legumemine/portal.do?class=Gene&externalids=' . $gensp.$gene_name,
            'text' => 'View LegumeMine gene report for : ' . $gene_name,
        );
    }
    if ($genus == 'Glycine' && $species == 'max') {

        $link[] = array(
            'href' => 'http://www.soybase.org/sbt/search/search_results.php?category=FeatureName&search_term=' . $gene_name,
            'text' => 'view at SoyBase: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4433&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://soykb.org/gene_card.php?gene=' . $gene_name,
            'text' => 'view at SoyKB: ' . $gene_name,
        );

/*
        $link[] = array(
            'href' => 'http://www.genomicus.biologie.ens.fr/genomicus-plants/cgi-bin/search.pl?view=default&amp;query=' . $gene_name,
            'text' => 'view at Genomicus: ' . $gene_name,
        );
*/
    }
    else if ($genus == 'Phaseolus' && $species == 'vulgaris') {
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=5084&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
        $link[] = array(
            'href' => '/genomes/gbrowse/Pv1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Pv1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Phaseolus' && $species == 'lunatus') {
        $link[] = array(
            'href' => 'https://phytozome-next.jgi.doe.gov/report/gene/Plunatus_V1/' . $gene_name . '.v1',
            'text' => 'view at Phytozome: ' . $gene_name,
        );
        $link[] = array(
            'href' => '/genomes/gbrowse/phalu.G27455.gnm1?query=name=phalu.' . $gene_name,
            'text' => 'view at LIS Pl1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Lotus' && $species == 'japonicus') {
        $link[] = array(
            'href' => 'https://lotus.au.dk/view/gene/' . $gene_name,
            'text' => 'view at Lotus Base: ' . $gene_name,
        );
        $link[] = array(
            'href' => '/genomes/gbrowse/Lj3.0?query=name=' . $gene_name,
            'text' => 'view at LIS Lj3.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Cicer' && $species == 'arietinum_CDCFrontier') {
        $gene_name = preg_replace('/^CDCFrontier\./','',$gene_name);
        $link[] = array(
            'href' => '/genomes/gbrowse/cicar.CDCFrontier.v1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Ca1.0 GBrowse: ' . $gene_name,
        );
        /*
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/chickpeamine/portal.do?class=Gene&externalids=CDCFrontier.'. $gene_name,
            'text' => 'view in ChickpeaMine: ' . $gene_name ,
        );
        */
        $link[] = array(
            'href' => 'https://legumefederation.org/gcv/phytozome_10_2/search/lis/cicar.CDCFrontier.' . $gene_name . '?sources=lis%2Ccicer&regexp=cicre|cicar|medtr&order=edit',
            'text' => 'Look for Cicer reticulatum microsyntenic region for ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=5236&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Cicer' && $species == 'arietinum_ICC4958') {
        $link[] = array(
            'href' => '/genomes/gbrowse/cicar.ICC4958.v2.0?query=name=cicar.' . $gene_name,
            'text' => 'view at LIS Cicer arietinum ICC 4958 genome v2.0 GBrowse: ' . $gene_name,
        );
        /*
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/chickpeamine/portal.do?class=Gene&externalids='. $gene_name,
            'text' => 'view in ChickpeaMine: ' . $gene_name,
        );
        */
        $link[] = array(
            'href' => 'https://legumefederation.org/gcv/phytozome_10_2/search/lis/cicar.' . $gene_name . '?sources=lis%2Ccicre&regexp=cicre|cicar|medtr&order=edit',
            'text' => 'Look for Cicer reticulatum microsyntenic region for ' . $gene_name,
        );
    }
    else if ($genus == 'Cajanus' && $species == 'cajan') {
        #hack: from the trees, the _gene is missing, but from the context viewer it is present
    $gene_name = preg_replace('/_gene$/','',$gene_name);
        $link[] = array(
            'href' => '/genomes/gbrowse/Cc1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Cc1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Trifolium' && $species == 'pratense') {
        $link[] = array(
            'href' => '/genomes/gbrowse/Tp2.0?query=name=' . $gene_name,
            'text' => 'view at LIS Tp2.0 GBrowse: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=5081&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Vigna' && $species == 'unguiculata') {
        $link[] = array(
            'href' => '/genomes/gbrowse/vigun.IT97K-499-35.gnm1?query=name=' . $gene_name,
            'text' => 'view at LIS vigun.IT97K-499-35.gnm1 GBrowse: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=5235&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Vigna' && $species == 'angularis') {
        $link[] = array(
            'href' => '/genomes/gbrowse/Va3.0?query=name=' . $gene_name,
            'text' => 'view at LIS Va3.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Lupinus' && $species == 'angustifolius') {
        $link[] = array(
            'href' => '/genomes/gbrowse/La1.0?query=name=' . $gene_name,
            'text' => 'view at LIS La1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Lupinus' && $species == 'albus') {
        $link[] = array(
            'href' => 'https://www.whitelupin.fr/browser/?data=Lupinus_albus&tracks=DNA%2Cmain_annotation&highlight=&loc=' . $gene_name,
            'text' => 'view at whitelupin.fr JBrowse: ' . $gene_name,
        );
        $link[] = array(
            'href' => '/genomes/gbrowse/lupal.Amiga.gnm1?query=name=lupal.' . $gene_name,
            'text' => 'view at LIS GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Vigna' && $species == 'radiata') {
        $link[] = array(
            'href' => '/genomes/gbrowse/Vr1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Vr1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Medicago' && $species == 'truncatula') {
        $gene_name = preg_replace('/^A17_HM341\./','',$gene_name);
        $link[] = array(
            'href' => 'http://medicmine.jcvi.org/medicmine/portal.do?class=Gene&externalids=' . $gene_name ,
            'text' => 'View MedicMine page for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.medicagogenome.org/feature/' . $gene_name ,
            'text' => 'View MTGD Mt4.0 Tripal page for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.medicagogenome.org/jbrowse/?data=medicago&loc='.$transcript_name,
            'text' => 'View at MTGD Mt4.0 JBrowse : ' . $transcript_name,
        );
        $link[] = array(
            'href' => '/genomes/gbrowse/Mt4.0?query=name=' . $gene_name,
            'text' => 'view at LIS Mt4.0 GBrowse: ' . $gene_name,
        );
/*
        $link[] = array(
            'href' => 'http://medtr.comparative-legumes.org/gb2/gbrowse/Mt3.5.1?name=' . $gene_name,
            'text' => 'view at JLIS Mt3.5.1 GBrowse: ' . $gene_name,
        );
*/
        $link[] = array(
            'href' => 'http://www.medicagohapmap.org/fgb2/gbrowse/mt40/?name=' . $gene_name,
            'text' => 'view at Mt HapMap: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4432&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
/*
        $link[] = array(
            'href' => 'http://plantgrn.noble.org/LegumeIP/getseq.do?dataset_id=4&seq_acc=IMGA|' . $transcript_name,
            'text' => 'view at LegumeIP: ' . $transcript_name,
        );
*/
        $link[] = array(
            'href' => 'http://www.ncbi.nlm.nih.gov/gene/?term=' . $gene_name,
            'text' => 'view at NCBI Gene: ' . $gene_name,
        );
/*
        $link[] = array(
            'href' => 'http://www.genomicus.biologie.ens.fr/genomicus-plants/cgi-bin/search.pl?view=default&amp;query=MTR_' . str_replace("Medtr","", $gene_name),
            'text' => 'view at Genomicus: ' . $gene_name,
        );
*/
    }
    else if ($genus == 'Arachis' && $species == 'duranensis') {
        $link[] = array(
            'href' => 'http://peanutbase.org/gbrowse_aradu1.0?query=name=' . $gene_name . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene_name,
        );            
    }
    else if ($genus == 'Arachis' && $species == 'ipaensis') {
        $link[] = array(
            'href' => 'http://peanutbase.org/gbrowse_araip1.0?query=name=' . $gene_name . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene_name,
        );        
    }
    else if ($genus == 'Arachis' && $species == 'hypogaea') {
        $browser_name = 'Arahy.'.$gene_name;
        $gene_name = 'arahy.'.$gene_name;
        $link[] = array(
            'href' => 'https://peanutbase.org/gbrowse_peanut1.0?query=name=' . $browser_name, // . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $browser_name,
        );        
    }
    else if ($genus == 'Pisum' && $species == 'sativum') {
        $browser_name = preg_replace('/^Cameor.gnm1.ann1./','',$gene_name);
        $link[] = array(
            'href' => 'https://urgi.versailles.inra.fr/jbrowse/gmod_jbrowse/?data=myData%2FPea%2FPsat_v1a%2Fdata&loc='.$browser_name.'&tracks=DNA%2Cgaps%2Cannotation.v1a%2CLD.cultivars%2CLD.landrace%2Cannotation.eugene%2Cannotation.v1a%2CRepeat.REPET&highlight=',
            'text' => 'view at INRA JBrowse: ' . $browser_name,
        );        
    }
    else if ($genus == 'Aeschynomene' && $species == 'evenia') {
        #$browser_name = preg_replace('/^aesev.CIAT22838.gnm1.ann0./','',$gene_name);
        $link[] = array(
            'href' => '/genomes/gbrowse/gb2/gbrowse/aesev.CIAT22838.gnm1?query=name=' . $gene_name,
            'text' => 'view at LIS GBrowse: ' . $gene_name,
        );        
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/jointvetchmine/gene:' . $gene_uniquename,
            'text' => 'view at JointvetchMine: ' . $gene_name,
        );        
    }
    else if ($genus == 'Medicago' && $species == 'sativa') {
        #$browser_name = preg_replace('/^medsa.XinJiangDaYe.gnm1.ann1./','',$gene_name);
        $link[] = array(
            'href' => '/genomes/gbrowse/gb2/gbrowse/medsa.XinJiangDaYe.gnm1/?query=name=medsa.' . $gene_name,
            'text' => 'view at LIS GBrowse: ' . $gene_name,
        );        
        //temporary hack until Sam gets with the dots program
        $gene_uniquename = preg_replace('/MS\./', 'MS_', $gene_uniquename);
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/medicmine/gene:' . $gene_uniquename,
            'text' => 'view at MedicMine: ' . $gene_name,
        );        
    }
    else if (($genus == 'Arabidopsis' && $species == 'thaliana') || preg_match('/^arath/',$gensp)) {
        if (!isset($gene_name)) {
            $gene_name = preg_replace('/\.\d+$/','',$transcript_name);
        }
        $gene_name=preg_replace('/^Col\./','',$gene_name);
        $context_viewer_name = 'arath.Col.'.$gene_name;
        $link[] = array(
            'href' => 'http://legumefederation.org/gcv/phytozome_10_2/search/lfo/' . $context_viewer_name,
            'text' => 'Find similar genomic contexts at LegFed for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.araport.org/locus/' . $gene_name,
            'text' => 'view at Arabidopsis Information Portal: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.arabidopsis.org/servlets/TairObject?type=locus&name=' . $gene_name,
            'text' => 'view at TAIR: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2296&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Zea' && $species == 'mays') {
        $gene_name = preg_replace('/_P\d+$/','',$transcript_name);
        $link[] = array(
            'href' => 'http://maizegdb.org/gene_center/gene?id=' . $gene_name,
            'text' => 'view at MaizeGDB: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.gramene.org/Zea_mays/Gene/Summary?g=' . $gene_name,
            'text' => 'view at Gramene: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4431&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Oryza' && $species == 'sativa') {
        $gene_name = preg_replace('/\.\d+$/','',$transcript_name);
        $link[] = array(
            'href' => 'http://rice.plantbiology.msu.edu/cgi-bin/gbrowse/rice/?name=' . $gene_name,
            'text' => 'view at MSU: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=5017&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Vitis' && $species == 'vinifera') {
        $gene_name = preg_replace('/\.\d+$/','',$transcript_name);
        $link[] = array(
            'href' => 'http://www.genoscope.cns.fr/cgi-bin/ggb/vitis/12X/gbrowse/vitis/?name=' . $gene_name,
            'text' => 'view at Genoscope: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2299&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Solanum' && $species == 'lycopersicum') {
        $gene_name = preg_replace('/\.\d+$/','',$transcript_name);
        $link[] = array(
            'href' => 'http://solgenomics.net/jbrowse/current/?data=data%2Fjson%2FSL2.50&loc=' . $gene_name . '&tracks=DNA%2Cgene_models&highlight=',
            'text' => 'view at Sol Genomics Network: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3308&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Prunus' && $species == 'persica') {
        $gene_name = preg_replace('/\.\d+$/','',$transcript_name);
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4909&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Amborella' && $species == 'trichopoda') {
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4851&searchText=' . $transcript_name,
            'text' => 'view at Phytozome: ' . $transcript_name,
        );
    }

    set_response_headers();
    drupal_json_output(handle_relative_links($link));  
    drupal_exit();
}


/*
 * the api endpoint /famreps_links?famreps=feature1,feature2,etc...
 * is served by this function famreps_linkout_json().
 * accepts GET or POST.
 * Note: if using POST, use content-type:application/x-www-form-urlencoded
 * because PHP 5.6 will barf upon content-type:application/json.
 * see https://www.bram.us/2014/10/26/php-5-6-automatically-populating-http_raw_post_data-is-deprecated-and-will-be-removed-in-a-future-version/
 */ 
function famreps_linkout_json() {
    if($_SERVER['REQUEST_METHOD'] == 'POST') {
        // if content was posted sent with application/json, this would be best:
        // $encoded = file_get_contents("php://input", TRUE);
        // $q = drupal_json_decode($encoded, TRUE);
        // however in the workaround described above, we are forced
        // use x-www-form-urlencoded
        $encoded = filter_input(INPUT_POST, 'famreps', FILTER_SANITIZE_ENCODED);
        $q = explode('%2C', $encoded);
    }
    else {
        $encoded = filter_input(INPUT_GET, 'famreps', FILTER_SANITIZE_ENCODED);
        $q = explode('%2C', $encoded);
    }
    
    //find the gene by use of the "family representative" (ie label
    //used for the phylonode, typically a polypeptide name (we stole
    //this trick from the phylotree module)
    $sql = <<<SQL
          SELECT f.name AS gene_name
          FROM chado.featureprop fp, chado.feature f
          WHERE fp.type_id = (SELECT cvterm_id FROM chado.cvterm
                              WHERE NAME = 'family representative')
          AND fp.value IN (:famreps)
          AND fp.feature_id = f.feature_id
SQL;
    // use chado_query(template, args) which 'will make sure that the
    // user input is santized and safe through type-checking and
    // escaping.'
    $args = array(':famreps' => $q);
    $result = chado_query($sql, $args);
    $genes = $result->fetchCol();
    if ($genes) {
        $gene_list = implode(",", $genes);
        $link[] = array(
            'href' => '/lis_context_viewer/multi/'.$gene_list,
            'text' => 'View Multi-aligned Genome Contexts for given genes',
            'famreps' => implode(',', $q)
        );
        $gene_list = implode("%0A", $genes);
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/legumemine/bag.do?subtab=upload&type=Gene&text='.$gene_list,
            'text' => 'Create List in LegumeMine for given Genes',
        );
        //in theory whether we have legume proteins in the input might be
        //independent of whether we can resolve legume genes from it; in
        //practice, it is not.
        $link[] = array(
            'href' => 'https://mines.legumeinfo.org/legumemine/bag.do?subtab=upload&type=Protein&text='.implode("%0A", $q),
            'text' => 'Create List in LegumeMine for given Proteins',
        );
    }
    $arath_list = array_map(function($value) {
        return preg_replace('/arath\.([A-Z0-9]+)\.[0-9]*/', '$1', $value);
        }, array_filter($q, function($value) {
            return (substr($value,0,6) === 'arath.' ? true : false);
        }));
    if (count($arath_list) > 0) {
        $link[] = array(
            'href' => 'https://apps.araport.org/thalemine/bag.do?subtab=upload&type=Gene&text='.implode("%0A", $arath_list),
            'text' => 'Create List in ThaleMine for given Genes',
        );
    }
    $zeama_list = array_map(function($value) {
        return preg_replace('/zeama\.([A-Z0-9]+)_P[0-9]*$/', '$1', $value);
        }, array_filter($q, function($value) {
            return (substr($value,0,6) === 'zeama.' ? true : false);
        }));
    if (count($zeama_list) > 0) {
        $link[] = array(
            'href' => 'http://maizemine.rnet.missouri.edu:8080/maizemine/bag.do?subtab=upload&type=Gene&text='.implode("%0A", $zeama_list),
            'text' => 'Create List in MaizeMine for given Genes',
        );
    }
    set_response_headers();
    drupal_json_output(handle_relative_links($link));
    drupal_exit();
}

?>
