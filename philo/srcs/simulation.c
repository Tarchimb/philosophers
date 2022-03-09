/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tarchimb <tarchimb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 07:59:25 by tarchimb          #+#    #+#             */
/*   Updated: 2022/03/09 12:53:07 by tarchimb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

/*
	Routine called by threads :
		>Init time of simulation
		>Call action's eating, sleeping, thinking
*/
void	*routine(void *philo_n)
{
	t_philo			*philo;

	philo = (t_philo *)philo_n;
	gettimeofday(&philo->start, NULL);
	if (philo->philo_position % 2 == 0)
		usleep(1000);
	while (1)
	{
		pthread_mutex_lock(&philo->mutex_alive);
		if (philo->alive == 0)
		{
			pthread_mutex_unlock(&philo->mutex_alive);
			break ;
		}
		pthread_mutex_unlock(&philo->mutex_alive);
		take_fork(philo);
		sleeping(philo);
		thinking(philo);
	}
	return (philo_n);
}

static int	destroy_mutex(t_prg *prg)
{
	int	i;

	i = 0;
	while (i < prg->numbers_of_philo)
	{
		if (pthread_mutex_destroy(&prg->philo[i].fork) != 0)
			return (-1);
		if (pthread_mutex_destroy(&prg->philo[i].mutex_talk) != 0)
			return (-1);
		if (pthread_mutex_destroy(&prg->philo[i].mutex_alive) != 0)
			return (-1);
		if (pthread_mutex_destroy(&prg->philo[i].mutex_last_meal) != 0)
			return (-1);
		if (pthread_mutex_destroy(&prg->philo[i].mutex_numbers_of_eats_needed)
			!= 0)
			return (-1);
		i++;
	}
	return (1);
}

static int	init_mutex(t_prg *prg, int i)
{
	while (++i < prg->numbers_of_philo)
	{
		if (pthread_mutex_init(&prg->philo[i].mutex_talk, NULL) != 0)
			return (-1);
		if (pthread_mutex_init(&prg->philo[i].fork, NULL) != 0)
			return (-1);
		if (pthread_mutex_init(&prg->philo[i].mutex_alive, NULL) != 0)
			return (-1);
		if (pthread_mutex_init(&prg->philo[i].mutex_last_meal, NULL) != 0)
			return (-1);
		if (pthread_mutex_init(&prg->philo[i].mutex_numbers_of_eats_needed,
				NULL) != 0)
			return (-1);
		prg->philo[i].left_fork = &prg->philo[i].fork;
	}
	i = -1;
	while (++i < prg->numbers_of_philo - 1)
		if (i < prg->numbers_of_philo - 1)
			prg->philo[i].right_fork = &prg->philo[i + 1].fork;
	prg->philo[i].right_fork = &prg->philo[0].fork;
	return (0);
}

/*
	control function for threads :
		>start simulation
		>create threads
		>init mutex and assign it to philosopher's
		>close simulation when needed
*/
int	start_simulation(t_prg *prg)
{
	int	i;

	i = -1;
	if (init_mutex(prg, i) == -1)
		return (-1);
	while (++i < prg->numbers_of_philo)
		gettimeofday(&prg->philo[i].last_meal, NULL);
	i = -1;
	while (++i < prg->numbers_of_philo)
		if (pthread_create(&prg->philo[i].philo, NULL, &routine
				, &prg->philo[i]) != 0)
			return (0);
	alive(prg);
	if (destroy_mutex(prg) == -1)
		return (0);
	return (1);
}
