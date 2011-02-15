bin_PROGRAMS = afscale deconv_dcon deconv_prep genlaue intlaue lchk \
newlaue spotin unscram \
afstats deconv_comb deconv_mmod \
difflaue difflmrg exchge genldm intanal lauediff lauelam \
lauenorm lauescale ldmge pslaue mtzdump mtztona4 na4tomtz \
tidytext \
intldm lauegen lscale \
filmps gnomconv reorder

FLIBS =@FLIBS@
LIBS = $(CCP4_LIBS) $(XDL_LIB) -ltiff

LDADD = ../lib/src/liblaue.la

afscale_SOURCES = afscale.f implicit_none.finc
afstats_SOURCES = afstats.f
deconv_comb_SOURCES = deconv_comb.f
deconv_dcon_SOURCES = deconv_dcon.f implicit_none.finc overlaps.finc
deconv_mmod_SOURCES = deconv_mmod.f
deconv_prep_SOURCES = deconv_prep.f implicit_none.finc overlaps.finc
difflaue_SOURCES = difflaue.f
difflmrg_SOURCES = difflmrg.f
exchge_SOURCES = exchge.f
filmps_SOURCES = filmps.c
genlaue_SOURCES = genlaue.f implicit_none.finc reflexions_def.finc novmax_def.finc reflexions.finc afilmspots.finc backgr.finc \
fiducials.finc sizes.finc cell_params.finc crystal_params.finc film_params.finc observed_spots.finc gen_file_header.finc \
flmconsts.finc refined_film_params.finc ramask.finc scnlim.finc
genldm_SOURCES = genldm.f
gnomconv_SOURCES = gnomconv.c
intanal_SOURCES = intanal.f
intlaue_SOURCES = intlaue.f dimgns.finc combck.finc comfid.finc comhat.finc comlaue.finc commask.finc commaskl.finc \
commcs.finc comori.finc comprf.finc compro.finc comras.finc comrep.finc comrfs.finc comscn.finc comscd.finc comtem.finc \
comuab.finc comhis.finc
intldm_SOURCES = intldm.f implicit_none.finc ldm_limits.finc ldm_prog_flags.finc 
lauediff_SOURCES = lauediff.f
lauegen_SOURCES = lauegen.f implicit_none.finc lauegen_limits.finc screen_params.finc view_handles.finc ip1_entries.finc  ip2_entries.finc \
film_pack.finc prog_flags.finc spot_box.finc file_units.finc cell_input.finc process_omit.finc spot_data.finc lauegen_observed_spots.finc \
image.finc selrefs.finc matches.finc symbol_types.finc refine_params.finc conics.finc lauegen_auto_params.finc spot_size.finc get_soft.finc \
auto_soft.finc bg_progress.finc upd_flags.finc autorefln_flags.finc integ.finc lirl_pars.finc save_pkpl.finc
lauelam_SOURCES = lauelam.f
lauenorm_SOURCES = lauenorm.f
lauescale_SOURCES = lauescale.f
lchk_SOURCES = lchk.f implicit_none.finc chanel.finc
ldmge_SOURCES = ldmge.f
lscale_SOURCES = lscale.f implicit_none.finc lscale_limits.finc lscale_params.finc
mtzdump_SOURCES = mtzdump.f
mtztona4_SOURCES = mtztona4.f
na4tomtz_SOURCES = na4tomtz.f
newlaue_SOURCES = newlaue.f implicit_none.finc newlaue_auto_params.finc pgstat.finc progdim.finc xyz.finc xysav.finc graph.finc radc.finc \
matrix.finc repeat.finc systyp.finc sys.finc subp.finc data.finc spots.finc point.finc vsave.finc histo.finc project.finc \
laues.finc lambda.finc
pslaue_SOURCES = pslaue.f
spotin_SOURCES = spotin.f implicit_none.finc sizes.finc fiducials.finc observed_spots.finc gen_file_header.finc filmconsts.finc \
film_params.finc
reorder_SOURCES = reorder.c
tidytext_SOURCES = tidytext.f
unscram_SOURCES = unscram.f implicit_none.finc overlaps.finc

filmps_LDADD = $(FLIBS)
gnomconv_LDADD = $(FLIBS)
reorder_LDADD = $(FLIBS)
