import React from 'react'
import { css } from 'glamor'
import { useSelector } from 'react-redux'
import { Link } from 'react-router-dom'
import { Check, Ratings, Plus, CrystalBall } from './Icons'

const links = [
	// {
	// 	url: '/',
	// 	label: 'Findings',
	// 	icon: BrainChart,
	// },
	{
		url: '/',
		label: 'Explain',
		icon: CrystalBall,
	},
	{
		url: '/results',
		label: 'Fortunes',
		icon: Ratings,
	},
	{
		url: '/vote',
		label: 'Approve',
		icon: Check,
	},
	{
		url: '/add',
		label: 'Create',
		icon: Plus,
	},
]

export default () => {
	const theme = useSelector(state => state.theme) || []

	return (
		<section
			{...css({
				zIndex: '99',
				position: 'fixed',
				bottom: 0,
				left: 0,
				width: '100%',
				padding: '1rem 0 2rem 0',
				fontSize: '1rem',
				[`@media(min-width: ${theme.breaks.menu}px)`]: {
					fontSize: '2rem',
					padding: '2rem 0',
				},
				backgroundColor: '#000000',
				borderTop: `.25rem solid rgba(${theme.backgroundColor}, .7)`,
			})}>
			<ul {...css({ listStyle: 'none', margin: 0, padding: 0, textAlign: 'center' })}>
				{links &&
					links.map(link => (
						<li
							key={link.url}
							{...css({
								width: '24%',
								display: 'inline-block',
								// paddingLeft: '1.5rem',
								// marginLeft: '1.5rem',
								borderLeft: `1px solid rgba(${theme.backgroundColor}, .4)`,
								[`@media(min-width: ${theme.breaks.menu}px)`]: {
									borderColor: `rgba(${theme.backgroundColor}, .8)`,
								},
								'&:nth-child(1)': {
									paddingLeft: 0,
									marginLeft: 0,
									borderLeft: 'none',
								},
							})}>
							<Link
								to={link.url}
								{...css({
									textDecoration: 'none',
									textTransform: 'uppercase',
									textAlign: 'center',
									tapHighlightColor: '#FFF',
									color: theme.background,
									[`@media(min-width: ${theme.breaks.menu}px)`]: {
										display: 'flex',
										alignItems: 'center',
										justifyContent: 'center',
									},
									'&:active': {
										color: '#FFF',
										'& path': {
											fill: '#FFF',
										},
										'& img': {
											filter: 'invert(50%)',
										},
									},
									'@media (hover: hover) and (pointer: fine)': {
										'&:hover': {
											color: '#FFF',
											'& path': {
												fill: '#FFF',
											},
											'& img': {
												filter: 'invert(50%)',
											},
										},
										'&:active': {
											color: '#FFF',
											'& path': {
												fill: '#FFF',
											},
											'& img': {
												filter: 'invert(50%)',
											},
										},
									},
								})}>
								<link.icon
									css={{
										display: 'block',
										margin: '0 auto .25rem auto',
										height: '1.75rem',
										maxWidth: '3.5rem',
										'& path': {
											fill: theme.background,
										},
										[`@media(min-width: ${theme.breaks.menu}px)`]: {
											display: 'inline-block',
											margin: '0 .5rem 0 0',
											maxWidth: '3.5rem',
										},
									}}
								/>

								<span>{link.label}</span>
							</Link>
						</li>
					))}
			</ul>
		</section>
	)
}
